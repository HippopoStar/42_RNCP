
#include "ft_ping.h"

static void
ft_ping_usage(void)
{
	ft_log_error(
		"Usage: ping [OPTIONS...] HOST ...\n"
		"Send ICMP ECHO_REQUEST packets to network hosts.\n"
		"\n"
		"Options:\n"
		"  -f, --flood             flood ping" /* " (root only)" */ "\n"
		"  -h, --help              give this help list\n"
		"      --ip-timestamp=FLAG IP timestamp of type FLAG, which is one of \"tsonly\" and \"tsaddr\"\n"
		"  -l, --preload=NUMBER    send NUMBER packets as fast as possible before falling into normal mode of behavior" /* " (root only)" */ "\n"
		"  -n, --numeric           do not resolve host addresses\n"
		"  -p, --pattern=PATTERN   fill ICMP packet with given pattern (hex)\n"
		"  -r, --ignore-routing    send directly to a host on an attached network\n"
		"  -s, --size=NUMBER       send NUMBER data octets\n"
		"  -T, --tos=NUM           set type of service (TOS) to NUM\n"
		"      --ttl=N             specify N as time-to-live\n"
		"  -v, --verbose           verbose output\n"
		"  -w, --timeout=N         stop after N seconds\n"
		"  -W, --linger=N          number of seconds to wait for response\n"
	);
}

int
main(int argc, char **argv)
{
	int    usage;
	int    has_error;
	int    c;
	int    option_index;
	t_args args;

	ft_log_init(argv[0]);
	init_args(&args);
	usage = 0;
	has_error = 0;
	while (
		!(
			has_error
			|| -1 == (c = getopt_long(argc, argv, "hvfl:nw:W:p:rs:T:", *get_long_options(), &option_index))
		)
	)
	{
		FT_LOG_DEBUG("option -%c", c);
		switch (c)
		{
			case 'h':
				usage = 1;
				break;
			case 'v':
				args.options |= OPT_VERBOSE;
				break;
			case 'f':
				args.options |= parse_optarg_flood(&args, &has_error);
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
				/* getopt_long already printed an error message. */
				has_error = 1;
				break;
			default:
				FT_LOG_ERROR("an unknown error occured");
				has_error = 1;
				break;
		}
	}
	// FT_LOG_DEBUG("option_index: %d", option_index); /* uninitialised */
	FT_LOG_DEBUG("optind: %d", optind);
	if (usage || has_error)
	{
		ft_ping_usage();
	}
	else if (optind == argc)
	{
		FT_LOG_ERROR("missing host operand");
	}
	else if (!has_error)
	{
		if (!(has_error = ping_args_setup(&args)))
		{
			print_args(&args);
			ping_process(&args, argc - optind, argv + optind, &has_error);
		}
		ping_args_teardown(&args);
	}
	return (has_error);
}
