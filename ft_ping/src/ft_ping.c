
#include "ft_ping.h"
#include "ft_ping_options.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

static void
init_args(t_args *args)
{
	args->preload = 0;
	args->timeout = -1;
	args->linger = MAXWAIT;
	args->pattern_len = MAXPATTERN;
	memset(args->pattern, '\0', MAXPATTERN);
	args->data_length = PING_DATALEN;
	args->tos = -1;
	args->ttl = 0;
	args->socket_type = 0;
	args->options = 0;
	args->suboptions = 0;
}

int
main(int argc, char **argv)
{
	int    c;
	int    option_index;
	t_args args;

	init_args(&args);
	while (!(-1 == (c = getopt_long(argc, argv, "vfl:nw:W:p:rs:T:", get_long_options(), &option_index))))
	{
		printf("option -%c\n", c);
		switch (c)
		{
			case 'v':
				break;
			case 'f':
				break;
			case 'l':
				break;
			case 'n':
				break;
			case 'w':
				break;
			case 'W':
				break;
			case 'p':
				break;
			case 'r':
				break;
			case 's':
				break;
			case 'T':
				break;
			case ARG_TTL:
				break;
			case ARG_IPTIMESTAMP:
				break;
			case '?':
				break;
			default:
				break;
		}
	}
	return (0);
}
