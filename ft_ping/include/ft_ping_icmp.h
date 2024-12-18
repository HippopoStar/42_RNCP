
#ifndef FT_PING_ICMP_H
# define FT_PING_ICMP_H

# include "ft_timespec.h"

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=libicmp/icmp.h;hb=HEAD#l139
*/
# define MAXIPLEN         60
# define MAXICMPLEN       76
# define ICMP_TSLEN       (8 + 3 * sizeof (n_time)) /* timestamp */

#endif /* FT_PING_ICMP_H */
