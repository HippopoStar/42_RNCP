
#include "ft_log.h"
#include "ft_ping_args.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Initializers.html
*/
void
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

void
print_args(t_args *args)
{
	size_t i;

	ft_log_debug(
		"{\n"
		"\t.preload = %lu\n"
		"\t.timeout = %d\n"
		"\t.linger = %d\n"
		"\t.pattern = [%." XSTR(MAXPATTERN) "s]\n"
		"\t.patptr = %p\n"
		"\t.pattern_len = %d\n"
		"\t.data_buffer = %p\n"
		"\t.data_length = %zu\n"
		"\t.tos = %d\n"
		"\t.ttl = %d\n"
		"\t.socket_type = %d\n"
		"\t.options = %#.8x\n"
		"\t.suboptions = %#.8x\n"
		"}\n",
		args->preload,
		args->timeout,
		args->linger,
		args->pattern,
		args->patptr,
		args->pattern_len,
		args->data_buffer,
		args->data_length,
		args->tos,
		args->ttl,
		args->socket_type,
		args->options,
		args->suboptions
	);
	if (!(NULL == args->patptr))
	{
		ft_log_debug("pattern content: [");
		for (i = 0; i < MAXPATTERN; i++)
		{
			ft_log_debug("%02hhx", args->pattern[i]);
		}
		ft_log_debug("]\n");
	}
	if (!(NULL == args->data_buffer))
	{
		ft_log_debug("data_buffer content: [");
		for (i = 0; i < args->data_length; i++)
		{
			ft_log_debug("%02hhx", args->data_buffer[i]);
		}
		ft_log_debug("]\n");
	}
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

unsigned int
parse_optarg_flood(t_args *args, int *has_error)
{
	if (is_root(args))
	{
		return (OPT_FLOOD);
	}
	else
	{
		FT_LOG_ERROR("flood behaviour requires super-user privileges");
		*has_error = 1;
		return (0);
	}
}

/*
** function parameter optarg shadows optarg from getopt.h
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Variables.html
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Scope.html
*/
unsigned long
parse_optarg_preload(t_args *args, const char *optarg, int *has_error)
{
	unsigned long n;
	char          *endptr;

	n = strtoul(optarg, &endptr, 0);
	if (*endptr || INT_MAX < n)
	{
		FT_LOG_ERROR("invalid preload value (%s)", optarg);
		*has_error = 1;
	}
	else if ((3 < n) && !is_root(args))
	{
		FT_LOG_ERROR("preload value shall not exceed 3 (super-user privileges required above that threshold)");
		*has_error = 1;
	}
	else
	{
		return (n);
	}
	return (0);
}

int
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
			FT_LOG_ERROR("error in pattern near %s", &optarg[off_acc]);
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

int
parse_optarg_ip_timestamp(const char *optarg, int *has_error)
{
	int sopt = 0;

	if (0 == strcasecmp(optarg, "tsonly"))
		sopt = SOPT_TSONLY;
	else if (0 == strcasecmp(optarg, "tsaddr"))
		sopt = SOPT_TSADDR;
	else
	{
		FT_LOG_ERROR("unsupported timestamp type: %s", optarg);
		*has_error = 1;
	}
	return (sopt);
}

size_t
parse_optarg_number(const char *optarg, size_t maxval, int allow_zero, int *has_error)
{
	char          *p;
	unsigned long n;

	n = strtoul(optarg, &p, 0);
	if (*p)
	{
		FT_LOG_ERROR("invalid value ('%s' near '%s')", optarg, p);
		*has_error = 1;
	}
	if (0 == n && !allow_zero)
	{
		FT_LOG_ERROR("option value too small: %s (min: 1)", optarg);
		*has_error = 1;
	}
	if (0 != maxval && maxval < n)
	{
		FT_LOG_ERROR("option value too big: %s (max: %zu)", optarg, maxval);
		*has_error = 1;
	}
	return n;
}
