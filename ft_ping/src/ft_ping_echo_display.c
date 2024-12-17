
#include "ft_ping_echo_display.h"

int
print_echo(
	int dupflag,
	struct ping_stat *ping_stat,
	struct sockaddr_in *dest __attribute__((unused)),
	struct sockaddr_in *from,
	struct ip *ip,
	struct icmp_header *icmp,
	int datalen
)
{
	(void)dupflag;
	(void)ping_stat;
	// (void)dest;
	(void)from;
	(void)ip;
	(void)icmp;
	(void)datalen;

	return (0);
}

void
print_icmp_header(
	struct sockaddr_in *from,
	struct ip *ip,
	struct icmp_header *icmp,
	int len
)
{
	(void)from;
	(void)ip;
	(void)icmp;
	(void)len;
}
