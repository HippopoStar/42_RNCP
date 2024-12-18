
#include "ft_log.h"
#include "ft_ping_data_setup_teardown.h"

#include <sys/socket.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void
ping_data_teardown(struct ping_data *p)
{
	/*
	** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping.c;hb=HEAD#l473
	*/
	ping_unset_data(p);
	if (!(p->ping_fd < 0))
	{
		/*
		** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/libping.c;hb=HEAD#l110
		*/
		close(p->ping_fd);
		p->ping_fd = -1;
	}
	/*
	** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l151
	*/
	if (!(NULL == p->ping_hostname))
	{
		free(p->ping_hostname);
		p->ping_hostname = NULL;
	}
}

int
ping_data_setup(struct ping_data *p, t_args *args, int *one)
{
	/*
	** Argument-agnostic
	*/

	/*
	** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping.c;hb=HEAD#l287
	** https://sourceware.org/git/?p=glibc.git;a=blob;f=locale/programs/xmalloc.c;hb=HEAD#l46
	*/
	if (!(ping_init(p, ICMP_ECHO, getpid())))
	{
		return (4);
	}
	/*
	** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping.c;hb=HEAD#l292
	** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.c;hb=HEAD#l195
	*/
	if ((ping_set_sockopt(p, SO_BROADCAST, (char *) one, sizeof (*one))) < 0)
	{
		return (4);
	}

	/*
	** Argument-dependant
	*/

	if (args->socket_type != 0)
	{
		if (ping_set_sockopt (p, args->socket_type, one, sizeof (*one)) < 0)
		{
			return (4);
		}
	}

	if (args->ttl > 0)
	{
		if (setsockopt (p->ping_fd, IPPROTO_IP, IP_TTL, &(args->ttl), sizeof (args->ttl)) < 0)
		{
			FT_LOG_ERROR("setsockopt(IP_TTL)");
			return (4);
		}
	}

	if (args->tos >= 0)
	{
		if (setsockopt (p->ping_fd, IPPROTO_IP, IP_TOS, &(args->tos), sizeof (args->tos)) < 0)
		{
			FT_LOG_ERROR("setsockopt(IP_TOS)");
			return (4);
		}
	}

	return (0);
}
