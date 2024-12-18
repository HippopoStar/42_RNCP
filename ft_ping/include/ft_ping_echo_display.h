
#ifndef FT_PING_ECHO_DISPLAY_H
# define FT_PING_ECHO_DISPLAY_H

# include "ft_ping_data.h"

/*
** https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/generic/netinet/ip.h;hb=HEAD#l266
*/
#define IPOPT_TS                68              /* timestamp */
/*
** https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/generic/netinet/ip.h;hb=HEAD#l276
** Offsets to fields in options other than EOL and NOP.
*/
#define IPOPT_OPTVAL            0               /* option ID */
#define IPOPT_OLEN              1               /* option length */
#define IPOPT_OFFSET            2               /* offset within option */
#define IPOPT_MINOFF            4               /* min value of above */
/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l49
*/
#define MAX_IPOPTLEN            40

int
print_echo(
	unsigned int options,
	int dupflag,
	struct ping_stat *ping_stat,
	struct sockaddr_in *dest,
	struct sockaddr_in *from,
	struct ip *ip,
	struct icmp_header *icmp,
	int datalen
);

int
print_icmp_header(
	unsigned int options,
	struct sockaddr_in *dest,
	struct sockaddr_in *from,
	struct ip *ip,
	struct icmp_header *icmp,
	int len
);

#endif /* FT_PING_ECHO_DISPLAY_H */
