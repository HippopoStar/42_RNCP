
#include "ft_log.h"
#include "ft_memory.h"
#include "ft_ping_data.h"
#include "ft_ping_icmp.h"

#include <sys/socket.h>

#include <errno.h>
#include <stdlib.h>

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.c;hb=HEAD#l222
*/
void
ping_unset_data(struct ping_data *p)
{
	if (!(NULL == p->ping_buffer))
	{
		free(p->ping_buffer);
		p->ping_buffer = NULL;
	}
	if (!(NULL == p->ping_cktab))
	{
		free(p->ping_cktab);
		p->ping_cktab = NULL;
	}
}

int
ping_setbuf(struct ping_data *p)
{
	if (NULL == p->ping_buffer)
	{
		/*
		** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.h;hb=HEAD#l79
		** _PING_BUFLEN (p, USE_IPV6)
		*/
		if (NULL == (p->ping_buffer = xmalloc ((MAXIPLEN + p->ping_datalen + ICMP_TSLEN) * sizeof(unsigned char))))
		{
			return (-1);
		}
	}
	if (NULL == p->ping_cktab)
	{
		if (NULL == (p->ping_cktab = xmalloc (p->ping_cktab_size * sizeof(char))))
		{
			return (-1);
		}
		memset (p->ping_cktab, 0, p->ping_cktab_size);
	}
	return (0);
}

int
ping_set_data(struct ping_data *p, void *data, size_t off, size_t len)
{
	struct icmp_header *icmp;

	if (ping_setbuf (p))
	{
		return -1;
	}
	if (p->ping_datalen < off + len)
	{
		return -1;
	}

	icmp = (struct icmp_header *) p->ping_buffer;
	memcpy (icmp->icmp_data + off, data, len);

	return 0;
}

void
ping_set_count(struct ping_data *ping, size_t count)
{
	ping->ping_count = count;
}

int
ping_set_sockopt(struct ping_data *ping, int opt, void *val, int valsize)
{
	int ret_val;

	if (!(0 == (ret_val = setsockopt (ping->ping_fd, SOL_SOCKET, opt, (char *) &val, valsize))))
	{
		FT_LOG_ERROR("Unable to set socket option");
	}
	return (ret_val);
}

int
ping_set_socket_fd(void)
{
	int             fd;
	struct protoent *proto;

	FT_LOG_DEBUG("ping_set_socket_fd");

	/* Initialize raw ICMP socket */
	proto = getprotobyname("icmp");
	if (!proto)
	{
		FT_LOG_ERROR("ping: unknown protocol icmp.");
		return (-1);
	}

	// fd = socket(AF_INET, SOCK_RAW, proto->p_proto);
	// FT_LOG_DEBUG("fd: %d", fd);
	// if (fd < 0)
	// {
	// 	if (errno == EPERM || errno == EACCES)
	// 	{
	// 		errno = 0;

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

			// useless_ident++; /* SOCK_DGRAM overrides our set identity. */ // TODO
	// 	}
	// 	else
	// 	{
	// 		FT_LOG_ERROR("Unable to open socket");
	// 		return (fd);
	// 	}
	// }
	return (fd);
}

void
ping_set_type(struct ping_data *ping, int type)
{
	ping->ping_type = type;
}

void
ping_set_event_handler(struct ping_data *ping, ping_efp pf, void *closure)
{
	ping->ping_event = pf;
	ping->ping_closure = closure;
}

void
ping_set_packetsize(struct ping_data *ping, size_t size)
{
	ping->ping_datalen = size;
}

int
ping_set_dest(struct ping_data *ping, const char *host)
{
#if HAVE_DECL_GETADDRINFO
	FT_LOG_DEBUG("HAVE_DECL_GETADDRINFO");
	int             rc;
	struct addrinfo hints, *res;
	char            *rhost;

# if defined HAVE_IDN || defined HAVE_IDN2
	FT_LOG_DEBUG("HAVE_IDN || HAVE_IDN2");
	rc = idna_to_ascii_lz(host, &rhost, 0); /* RHOST is allocated. */
	if (rc)
	{
		return (1);
	}
	host = rhost;
# else
	rhost = NULL;
# endif

	memset(&hints, '\0', sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_CANONNAME;
# ifdef AI_IDN
	FT_LOG_DEBUG("AI_IDN");
	hints.ai_flags |= AI_IDN;
# endif
# ifdef AI_CANONIDN
	FT_LOG_DEBUG("AI_CANONIDN");
	hints.ai_flags |= AI_CANONIDN;
# endif

	rc = getaddrinfo(host, NULL, &hints, &res);

	if (rc)
	{
		free(rhost);
		return (1);
	}

	memcpy(&(ping->ping_dest), res->ai_addr, res->ai_addrlen);
	if (res->ai_canonname)
	{
		ping->ping_hostname = strdup(res->ai_canonname);
	}
	else
	{
# if defined HAVE_IDN || defined HAVE_IDN2
		ping->ping_hostname = host;
# else
		ping->ping_hostname = strdup(host);
# endif
	}

	freeaddrinfo(res);

	return (0);
#else /* !HAVE_DECL_GETADDRINFO */

	struct sockaddr_in *s_in = &(ping->ping_dest);
	s_in->sin_family = AF_INET;
# ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
	FT_LOG_DEBUG("HAVE_STRUCT_SOCKADDR_IN_SIN_LEN");
	s_in->sin_len = sizeof(struct sockaddr_in);
# endif
	if (inet_aton(host, &s_in->sin_addr))
	{
		ping->ping_hostname = strdup(host);
	}
	else
	{
		struct hostent *hp;
# if defined HAVE_IDN || defined HAVE_IDN2
		char *rhost;
		int rc;

		rc = idna_to_ascii_lz(host, &rhost, 0);
		if (rc)
		{
			return (1);
		}
		hp = gethostbyname(rhost);
		free (rhost);
# else/* !HAVE_IDN && !HAVE_IDN2 */
		hp = gethostbyname(host);
# endif
		if (!hp)
		{
			return (1);
		}

		s_in->sin_family = hp->h_addrtype;
		if (hp->h_length > (int)sizeof(s_in->sin_addr))
		{
			hp->h_length = sizeof(s_in->sin_addr);
		}

		memcpy(&s_in->sin_addr, hp->h_addr, hp->h_length);
		ping->ping_hostname = strdup(hp->h_name);
	}
	return (0);
#endif /* !HAVE_DECL_GETADDRINFO */
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/libping.c;hb=HEAD#l61
*/
int
ping_init(struct ping_data *p, int type, int ident)
{
	/* Initialize PING structure to default values */
	memset(p, '\0', sizeof(struct ping_data));

	if ((p->ping_fd = ping_set_socket_fd()) < 0)
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

double
nabs(double a)
{
	return ((a < 0) ? -a : a);
}

double
nsqrt(double a, double prec)
{
	double x0, x1;

	if (a < 0)
	{
		return (0);
	}
	if (a < prec)
	{
		return (0);
	}
	x1 = a / 2;
	do
	{
		x0 = x1;
		x1 = (x0 + a / x0) / 2;
	}
	while (nabs(x1 - x0) > prec);

	return (x1);
}
