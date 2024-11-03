
#include "ft_ping_options.h"

#include <getopt.h>
#include <stdlib.h>

const struct option *
get_long_options(void)
{
	/* https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Initializers.html */
	static const struct option long_options[] =
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
	return (long_options);
}

// atoi
