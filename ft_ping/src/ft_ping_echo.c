
#include "ft_log.h"
#include "ft_ping_echo_display.h"
#include "ft_ping_echo.h"
#include "ft_ping_run.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l55
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l155
*/
static int
handler(
	int                code,
	void               *closure,
	struct sockaddr_in *dest,
	struct sockaddr_in *from,
	struct ip          *ip,
	struct icmp_header *icmp,
	int                datalen
)
{
	switch (code)
	{
		case PEV_RESPONSE:
		case PEV_DUPLICATE:
			print_echo(
				code == PEV_DUPLICATE,
				(struct ping_stat *)closure,
				dest,
				from,
				ip,
				icmp,
				datalen
			);
			break;
		case PEV_NOECHO:
			print_icmp_header(from, ip, icmp, datalen);
			break;
	}
	return (0);
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping.c;hb=HEAD#l508
*/
static int
ping_finish(struct ping_data *ping)
{
	fflush(stdout);
	printf("--- %s ping statistics ---\n", ping->ping_hostname);
	printf("%zu packets transmitted, ", ping->ping_num_xmit);
	printf("%zu packets received, ", ping->ping_num_recv);
	if (ping->ping_num_rept)
	{
		printf("+%zu duplicates, ", ping->ping_num_rept);
	}
	if (ping->ping_num_xmit)
	{
		if (ping->ping_num_recv > ping->ping_num_xmit)
		{
			printf("-- somebody is printing forged packets!");
		}
		else
		{
			printf(
				"%d%% packet loss",
				(int)(((ping->ping_num_xmit - ping->ping_num_recv) * 100) / ping->ping_num_xmit)
			);
		}
	}
	printf("\n");
	return (0);
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l61
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l626
*/
static int
echo_finish(t_args *args, struct ping_data *ping)
{
	ping_finish(ping);
	if (ping->ping_num_recv && PING_TIMING(args->data_length))
	{
		struct ping_stat *ping_stat = (struct ping_stat *)ping->ping_closure;
		double total = ping->ping_num_recv + ping->ping_num_rept;
		double avg = ping_stat->tsum / total;
		double vari = ping_stat->tsumsq / total - avg * avg;

		printf(
			"round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			ping_stat->tmin, avg, ping_stat->tmax, nsqrt(vari, 0.0005)
		);
	}
	return (ping->ping_num_recv == 0);
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l67
*/
int
ping_echo(t_args *args, struct ping_data *ping, const char *hostname)
{
	char rspace[MAX_IPOPTLEN]; /* Maximal IP option space.  */
	struct ping_stat ping_stat;
	int              status;

	memset(&ping_stat, '\0', sizeof(struct ping_stat));
	ping_stat.tmin = 999999999.0;

	ping_set_type(ping, ICMP_ECHO);
	ping_set_packetsize(ping, args->data_length);
	ping_set_event_handler(ping, handler, &ping_stat);

	if (ping_set_dest(ping, hostname))
	{
		FT_LOG_ERROR("unknown host");
		return (2);
	}
	if (args->options & OPT_IPTIMESTAMP)
	{
		int type;

		if (args->suboptions & SOPT_TSADDR)
		{
			type = IPOPT_TS_TSANDADDR;
		}
		else
		{
			type = IPOPT_TS_TSONLY;
		}

		memset(rspace, '\0', sizeof(MAX_IPOPTLEN * sizeof(char)));
		rspace[IPOPT_OPTVAL] = IPOPT_TS;
		rspace[IPOPT_OLEN] = sizeof (rspace);
		if (type != IPOPT_TS_TSONLY)
		{
			rspace[IPOPT_OLEN] -= sizeof (n_time); /* Exsessive part. */
		}
		rspace[IPOPT_OFFSET] = IPOPT_MINOFF + 1;

# ifdef IPOPT_POS_OV_FLG
		rspace[IPOPT_POS_OV_FLG] = type;
# else
		rspace[3] = type;
# endif/* !IPOPT_POS_OV_FLG */

		if (setsockopt(ping->ping_fd, IPPROTO_IP, IP_OPTIONS, rspace, rspace[IPOPT_OLEN]) < 0)
		{
			FT_LOG_ERROR("setsockopt");
			return (2);
		}
	}

	printf(
		"PING %s (%s): %zu data bytes",
		ping->ping_hostname,
		inet_ntoa(ping->ping_dest.sin_addr),
		args->data_length
	);
	if (args->options & OPT_VERBOSE)
	{
		printf(", id 0x%04x = %u", ping->ping_ident, ping->ping_ident);
	}

	printf("\n");

	status = ping_run(args, ping, echo_finish);
	free(ping->ping_hostname);
	return (status);
}
