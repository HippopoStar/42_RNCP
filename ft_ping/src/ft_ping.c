
#include "ft_ping.h"

#include <stdio.h>

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
			|| -1 == (c = getopt_long(argc, argv, "vfl:nw:W:p:rs:T:", *get_long_options(), &option_index))
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
			print_args(&args);
			ping_run(&args);
		}
		ping_teardown(&args);
	}
	return (0);
}
