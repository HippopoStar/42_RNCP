
#include "ft_ping.h"
#include "ft_ping_options.h"

#include <sys/socket.h>

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/File_002dScope-Variables.html
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/volatile.html
** https://www.gnu.org/software/libc/manual/html_node/Sigaction-Function-Example.html
*/
static volatile sig_atomic_t stop = 0;

static void
sig_int_handler(int signal)
{
	(void)signal;
	stop = 1;
}

static void
init_args(t_args *args)
{
	*args = (t_args) {
		0,            /* preload */
		-1,           /* timeout */
		MAXWAIT,      /* linger */
		{},           /* pattern */
		NULL,         /* patptr */
		MAXPATTERN,   /* pattern_len */
		NULL,         /* data_buffer */
		PING_DATALEN, /* data_length */
		-1,           /* tos */
		0,            /* ttl */
		0,            /* socket_type */
		0,            /* options */
		0             /* suboptions */
	};
}

static void
free_dynamically_allocated_memory(t_args *args)
{
	if (!(NULL == args->data_buffer))
	{
		free(args->data_buffer);
	}
}

/*
** data_buffer pointer must be initialized to NULL and passed by address.
** Returns NULL if data_length == 0 or on allocation failure.
*/
static unsigned char *
init_data_buffer(unsigned char **data_buffer, size_t data_length, unsigned char *patptr, size_t pattern_len)
{
	size_t i;

	if (0 < data_length)
	{
		if (NULL == (*data_buffer = (unsigned char *)malloc(data_length * sizeof(unsigned char))))
		{
			fprintf(stderr, "memory exhausted\n");
			return (NULL);
		}
		i = 0;
		while (i < data_length)
		{
			(*data_buffer)[i] = (NULL == patptr) ? i : patptr[i % pattern_len];
			i++;
		}
	}
	return (*data_buffer);
}

static int
is_root(t_args *args)
{
	return (
		(OPT_FLOOD & args->options)
		|| (3 < args->preload)
		|| (0 == getuid())
	);
}

/*
** function parameter optarg shadows optarg from getopt.h
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Variables.html
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Scope.html
*/
static unsigned long
parse_optarg_preload(t_args *args, const char *optarg, int *has_error)
{
	unsigned long n;
	char          *endptr;

	n = strtoul(optarg, &endptr, 0);
	if (*endptr || INT_MAX < n)
	{
		fprintf(stderr, "invalid preload value (%s)\n", optarg);
		*has_error = 1;
	}
	else if ((3 < n) && !is_root(args))
	{
		fprintf(stderr, "preload value shall not exceed 3 (super-user privileges required above that threshold)\n");
		*has_error = 1;
	}
	else
	{
		return (n);
	}
	return (0);
}

static int
parse_optarg_pattern(t_args *args, const char *optarg, int *has_error)
{
	int          i;
	int          off;
	int          off_acc;
	unsigned int c;

	off_acc = 0;
	i = 0;
	while (
		!(
			*has_error
			|| '\0' == optarg[off_acc]
		)
		&& i < args->pattern_len /* args.pattern_len is initialized with value MAXPATTERN */
	)
	{
		if (1 != sscanf(&optarg[off_acc], "%2x%n", &c, &off))
		{
			fprintf(stderr, "error in pattern near %s\n", &optarg[off_acc]);
			*has_error = 1;
		}
		args->pattern[i] = c;
		off_acc += off;
		i++;
	}
	if (!*has_error)
	{
		args->patptr = args->pattern;
		return (i);
	}
	return (0);
}

static int
parse_optarg_ip_timestamp(const char *optarg, int *has_error)
{
	int sopt = 0;

	if (0 == strcasecmp(optarg, "tsonly"))
		sopt = SOPT_TSONLY;
	else if (0 == strcasecmp(optarg, "tsaddr"))
		sopt = SOPT_TSADDR;
	else
	{
		fprintf(stderr, "unsupported timestamp type: %s\n", optarg);
		*has_error = 1;
	}
	return (sopt);
}

static size_t
parse_optarg_number(const char *optarg, size_t maxval, int allow_zero, int *has_error)
{
	char          *p;
	unsigned long n;

	n = strtoul(optarg, &p, 0);
	if (*p)
	{
		fprintf(stderr, "invalid value ('%s' near '%s')\n", optarg, p);
		*has_error = 1;
	}
	if (0 == n && !allow_zero)
	{
		fprintf(stderr, "option value too small: %s\n", optarg);
		*has_error = 1;
	}
	if (0 != maxval && maxval < n)
	{
		fprintf(stderr, "option value too big: %s\n", optarg);
		*has_error = 1;
	}
	return n;
}

static int
ping_setup(t_args *args)
{
	if (
		0 == args->data_length
		|| !(NULL == init_data_buffer(&(args->data_buffer), args->data_length, args->patptr, args->pattern_len))
	)
	{
		return (1);
	}
	return (0);
}

static void
ping_run(t_args *args)
{
	(void)args;

	struct sigaction old_action;
	struct sigaction new_action;

	new_action.sa_handler = sig_int_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;

	sigaction(SIGINT, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
	{
		sigaction (SIGINT, &new_action, NULL);
	}

	while(!stop)
	{
		write(1, ".", 1);
		sleep(1);
	}
	printf("\n");
}

int
main(int argc, char **argv)
{
	int    has_error;
	int    c;
	int    option_index;
	t_args args;

	init_args(&args);
	has_error = 0;
	while (
		!(
			has_error
			|| -1 == (c = getopt_long(argc, argv, "vfl:nw:W:p:rs:T:", get_long_options(), &option_index))
		)
	)
	{
		printf("option -%c\n", c);
		switch (c)
		{
			case 'v':
				args.options |= OPT_VERBOSE;
				break;
			case 'f':
				args.options |= OPT_FLOOD;
				break;
			case 'l':
				args.preload = parse_optarg_preload(&args, optarg, &has_error);
				break;
			case 'n':
				args.options |= OPT_NUMERIC;
				break;
			case 'w':
				args.timeout = parse_optarg_number(optarg, INT_MAX, 0, &has_error);
				break;
			case 'W':
				args.linger = parse_optarg_number(optarg, INT_MAX, 0, &has_error);
				break;
			case 'p':
				args.pattern_len = parse_optarg_pattern(&args, optarg, &has_error);
				break;
			case 'r':
				args.socket_type |= SO_DONTROUTE;
				break;
			case 's':
				args.data_length = parse_optarg_number(optarg, PING_MAX_DATALEN, 1, &has_error);
				break;
			case 'T':
				args.tos = parse_optarg_number(optarg, 255, 1, &has_error);
				break;
			case ARG_TTL:
				args.ttl = parse_optarg_number(optarg, 255, 0, &has_error);
				break;
			case ARG_IPTIMESTAMP:
				args.options |= OPT_IPTIMESTAMP;
				args.suboptions |= parse_optarg_ip_timestamp(optarg, &has_error);
				break;
			case '?':
				has_error = 1;
				break;
			default:
				fprintf(stderr, "an unknown error occured\n");
				has_error = 1;
				break;
		}
	}
	if (optind == argc)
	{
		fprintf(stderr, "missing host operand\n");
	}
	else if (!has_error)
	{
		if (ping_setup(&args))
		{
			ping_run(&args);
		}
		free_dynamically_allocated_memory(&args);
	}
	return (0);
}
