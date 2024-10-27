
#include "ft_ping.h"

#include <getopt.h>

/* Define keys for long options that do not have short counterparts. */
enum
{
	ARG_TTL = 256,
	ARG_IPTIMESTAMP,
};

int main(int argc, char **argv)
{
	(void)argc; // TODO
	(void)argv; // TODO
	static struct option long_options[] =
		{
			/* These options do not set a flag.
			*  We distinguish them by their indices. */
			{"verbose",        no_argument,       0, 'v'},
			{"flood",          no_argument,       0, 'f'},
			{"preload",        required_argument, 0, 'l'},
			{"numeric",        no_argument,       0, 'n'},
			{"timeout",        required_argument, 0, 'w'},
			{"linger",         required_argument, 0, 'W'},
			{"pattern",        required_argument, 0, 'p'},
			{"ignore-routing", no_argument,       0, 'r'},
			{"size",           required_argument, 0, 's'},
			{"tos",            required_argument, 0, 'T'},
			{"ttl",            required_argument, 0, ARG_TTL},
			{"ip-timestamp",   required_argument, 0, ARG_IPTIMESTAMP},
			{0, 0, 0, 0}
		};
	(void)long_options; // TODO
	return (0);
}
