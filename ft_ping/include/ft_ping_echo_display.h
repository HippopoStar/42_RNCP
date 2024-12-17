
#ifndef FT_PING_ECHO_DISPLAY_H
# define FT_PING_ECHO_DISPLAY_H

# include "ft_ping_data.h"

int
print_echo(
	int dupflag,
	struct ping_stat *ping_stat,
	struct sockaddr_in *dest,
	struct sockaddr_in *from,
	struct ip *ip,
	struct icmp_header *icmp,
	int datalen
);

void
print_icmp_header(
	struct sockaddr_in *from,
	struct ip *ip,
	struct icmp_header *icmp,
	int len
);

#endif /* FT_PING_ECHO_DISPLAY_H */
