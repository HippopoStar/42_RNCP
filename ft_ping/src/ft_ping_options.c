
#include "ft_ping_options.h"

#include <getopt.h>
#include <stdlib.h>

/*
** The disability for a function to return an array is explained through the
** consideration that from the caller perspective, the assignment operator would
** not copy an array content
** In practice, when the return statement of a function is composed with an
** array, the latter is coerced to a pointer to its zeroth element
** However, it is possible to return a pointer to an array
**
** To copy an array content through assignment, see:
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Structure-Assignment.html
*/
const struct option
(*get_long_options(void))[]
{
	/* https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Constructing-Array-Values.html */
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
	return (&long_options);
}
