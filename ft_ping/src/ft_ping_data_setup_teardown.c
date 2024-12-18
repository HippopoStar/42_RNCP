
#include "ft_log.h"
#include "ft_ping_data_setup_teardown.h"
#include "ft_timespec.h"

#include <sys/socket.h>

#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.c;hb=HEAD#l222
*/
static void
ping_unset_data(struct ping_data *p)
{
	if (p->ping_buffer)
	{
		free(p->ping_buffer);
		p->ping_buffer = NULL;
	}
	if (p->ping_cktab)
	{
		free(p->ping_cktab);
		p->ping_cktab = NULL;
	}
}

void
ping_data_teardown(struct ping_data *p)
{
	ping_unset_data(p);
	if (!(p->ping_fd < 0))
	{
		close(p->ping_fd); // TODO: ensure
		p->ping_fd = -1;
	}
	if (!(NULL == p->ping_hostname))
	{
		free(p->ping_hostname);
		p->ping_hostname = NULL;
	}
}

static int
ping_open_socket(void)
{
	int             fd;
	struct protoent *proto;

	/* Initialize raw ICMP socket */
	proto = getprotobyname("icmp");
	if (!proto)
	{
		FT_LOG_ERROR("ping: unknown protocol icmp.");
		return (-1);
	}

	fd = socket(AF_INET, SOCK_RAW, proto->p_proto);
	if (fd < 0)
	{
		if (errno == EPERM || errno == EACCES)
		{
			errno = 0;

			/*
			** At least Linux can allow subprivileged users to send ICMP
			** packets formally encapsulated and built as a datagram socket,
			** but then the identity number is set by the kernel itself.
			*/
			fd = socket(AF_INET, SOCK_DGRAM, proto->p_proto);
			if (fd < 0)
			{
				if (
					errno == EPERM
					|| errno == EACCES
					|| errno == EPROTONOSUPPORT
				)
				{
					FT_LOG_ERROR("ping: Lacking privilege for icmp socket.");
				}
				else
				{
					FT_LOG_ERROR("ping: %s", strerror(errno));
				}

				return (fd);
			}

			// useless_ident++; /* SOCK_DGRAM overrides our set identity. */ TODO
		}
		else
		{
			return (fd);
		}
	}
	return (fd);
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/libping.c;hb=HEAD#l61
*/
static int
ping_init(struct ping_data *p, int type, int ident)
{
	/* Initialize PING structure to default values */
	memset(p, '\0', sizeof(struct ping_data));

	if ((p->ping_fd = ping_open_socket()) < 0)
	{
		return (0);
	}
	p->ping_type = type;
	p->ping_count = 0;
	p->ping_interval = PING_DEFAULT_INTERVAL;
	p->ping_datalen = sizeof(struct icmp_header);
	/* Make sure we use only 16 bits in this field, id for icmp is a unsigned short. */
	p->ping_ident = ident & 0xFFFF;
	p->ping_cktab_size = PING_CKTABSIZE;
	p->ping_start_time = current_timespec();
	return (1);
}

int
ping_data_setup(struct ping_data *p)
{
	int ret_val;

	ret_val = ping_init(p, ICMP_ECHO, getpid());
	return (ret_val);
}
