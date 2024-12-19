
#include "ft_log.h"
#include "ft_ping_icmp.h"
#include "ft_ping_run_loop.h"

#include <arpa/inet.h>
#include <sys/select.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.h;hb=HEAD#l69
*/
#define PING_PRECISION         1000     /* Millisecond precision */
#define PING_SET_INTERVAL(t,i) do {                                     \
    (t).tv_sec = (i)/PING_PRECISION;                                    \
    (t).tv_nsec = ((i)%PING_PRECISION)*(1e9/PING_PRECISION) ;           \
  } while (0)

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.h;hb=HEAD#l132
*/
#define _C_BIT(p,bit)   (p)->ping_cktab[(bit)>>3]       /* byte in ck array */
#define _C_MASK(bit)    (1 << ((bit) & 0x07))
#define _C_IND(p,bit)   ((bit) % (8 * (p)->ping_cktab_size))

#define _PING_SET(p,bit)                                                \
  do                                                                    \
    {                                                                   \
      int n = _C_IND (p,bit);                                           \
      _C_BIT (p,n) |= _C_MASK (n);                                      \
    }                                                                   \
  while (0)

#define _PING_CLR(p,bit)                                                \
  do                                                                    \
    {                                                                   \
      int n = _C_IND (p,bit);                                           \
      _C_BIT (p,n) &= ~_C_MASK (n);                                     \
    }                                                                   \
  while (0)

#define _PING_TST(p,bit)                                                \
  (_C_BIT (p, _C_IND (p,bit)) & _C_MASK  (_C_IND (p,bit)))

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=libicmp/icmp_cksum.c;hb=HEAD#l24
*/
static unsigned short
icmp_cksum (unsigned char *addr, int len)
{
	int sum = 0;
	unsigned short answer = 0;
	unsigned short *wp;

	for (wp = (unsigned short *) addr; len > 1; wp++, len -= 2)
	{
		sum += *wp;
	}

	/* Take in an odd byte if present */
	if (len == 1)
	{
		*(unsigned char *) &answer = *(unsigned char *) wp;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);   /* add high 16 to low 16 */
	sum += (sum >> 16);                   /* add carry */
	answer = ~sum;                        /* truncate to 16 bits */
	return answer;
}

/*
** ping_recv
** #############################################################################
*/

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=libicmp/icmp_echo.c;hb=HEAD#l51
*/
static int
icmp_generic_decode (
	unsigned char *buffer,
	size_t bufsize,
	struct ip **ipp,
	struct icmp_header **icmpp
)
{
	size_t hlen;
	unsigned short cksum;
	struct ip *ip;
	struct icmp_header *icmp;

	/* IP header */
	ip = (struct ip *) buffer;
	hlen = ip->ip_hl << 2;
	if (bufsize < hlen + ICMP_MINLEN)
		return -1;

	/* ICMP header */
	icmp = (struct icmp_header *) (buffer + hlen);

	/* Prepare return values */
	*ipp = ip;
	*icmpp = icmp;

	/* Recompute checksum */
	cksum = icmp->icmp_cksum;
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = icmp_cksum ((unsigned char *) icmp, bufsize - hlen);
	if (icmp->icmp_cksum != cksum)
		return 1;
	return 0;
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/libping.c;hb=HEAD#l194
*/
static int
my_echo_reply (struct ping_data *p, struct icmp_header *icmp)
{
	struct ip *orig_ip = &icmp->icmp_ip;
	struct icmp_header *orig_icmp = (struct icmp_header *) (orig_ip + 1);

	return (
		orig_ip->ip_dst.s_addr == p->ping_dest.sin_addr.s_addr
		&& orig_ip->ip_p == IPPROTO_ICMP
		&& orig_icmp->icmp_type == ICMP_ECHO
		&& (ntohs (orig_icmp->icmp_id) == p->ping_ident /*|| useless_ident*/) // TODO
	);
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/libping.c;hb=HEAD#l206
** p->ping_event:
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.h;hb=HEAD#l87
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l84
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l155
*/
static int
ping_recv (struct ping_data *p, unsigned int options)
{
	socklen_t fromlen = sizeof (p->ping_from);
	int n, rc;
	struct icmp_header *icmp;
	struct ip *ip;
	int dupflag;

	n = recvfrom (
		p->ping_fd,
		(char *) p->ping_buffer,
		/*
		** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.h;hb=HEAD#l79
		** _PING_BUFLEN (p, USE_IPV6)
		*/
		(MAXIPLEN + p->ping_datalen + ICMP_TSLEN),
		0,
		(struct sockaddr *) &p->ping_from,
		&fromlen
	);
	if (n < 0)
		return -1;

	rc = icmp_generic_decode (p->ping_buffer, n, &ip, &icmp);
	if (rc < 0)
	{
		/*FIXME: conditional */
		fprintf (stderr, "packet too short (%d bytes) from %s\n", n, inet_ntoa (p->ping_from.sin_addr));
		return -1;
	}

	switch (icmp->icmp_type)
	{
		case ICMP_ECHOREPLY:
		// case ICMP_TIMESTAMPREPLY:
		// case ICMP_ADDRESSREPLY:
		/*    case ICMP_ROUTERADV: */

			if (ntohs (icmp->icmp_id) != p->ping_ident /*&& useless_ident == 0*/) // TODO
				return -1;

			if (rc)
				fprintf (stderr, "checksum mismatch from %s\n", inet_ntoa (p->ping_from.sin_addr));

			p->ping_num_recv++;
			if (_PING_TST (p, ntohs (icmp->icmp_seq)))
			{
				p->ping_num_rept++;
				p->ping_num_recv--;
				dupflag = 1;
			}
			else
			{
				_PING_SET (p, ntohs (icmp->icmp_seq));
				dupflag = 0;
			}

			if (p->ping_event)
				if (!(0 == (*p->ping_event) (
					options,
					dupflag ? PEV_DUPLICATE : PEV_RESPONSE,
					p->ping_closure,
					&p->ping_dest,
					&p->ping_from,
					ip,
					icmp,
					n
				)))
				{
					return (4);
				}
			break;

		case ICMP_ECHO:
		// case ICMP_TIMESTAMP:
		// case ICMP_ADDRESS:
			return -1;

		default:
			if (!my_echo_reply (p, icmp))
				return -1;

			if (p->ping_event)
				if (!(0 == (*p->ping_event) (
					options,
					PEV_NOECHO,
					p->ping_closure,
					&p->ping_dest,
					&p->ping_from,
					ip,
					icmp,
					n
				)))
				{
					return (4);
				}
	}
	return 0;
}

/*
** send_echo
** #############################################################################
*/

int
icmp_generic_encode (unsigned char *buffer, size_t bufsize, int type, int ident, int seqno)
{
	struct icmp_header *icmp;

	if (bufsize < ICMP_MINLEN)
		return -1;
	icmp = (struct icmp_header *) buffer;
	icmp->icmp_type = type;
	icmp->icmp_code = 0;
	icmp->icmp_cksum = 0;
	icmp->icmp_seq = htons (seqno);
	icmp->icmp_id = htons (ident);

	icmp->icmp_cksum = icmp_cksum (buffer, bufsize);
	return 0;
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=libicmp/icmp_echo.c;hb=HEAD#l82
*/
static int
icmp_echo_encode (unsigned char *buffer, size_t bufsize, int ident, int seqno)
{
	return icmp_generic_encode (buffer, bufsize, ICMP_ECHO, ident, seqno);
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/libping.c;hb=HEAD#l47
*/
static size_t
ping_packetsize (struct ping_data *p)
{
	// if (p->ping_type == ICMP_TIMESTAMP || p->ping_type == ICMP_TIMESTAMPREPLY)
	// {
	// 	return ICMP_TSLEN;
	// }

	// if (p->ping_type == ICMP_ADDRESS || p->ping_type == ICMP_ADDRESSREPLY)
	// {
	// 	return ICMP_MASKLEN;
	// }

	return (PING_HEADER_LEN + p->ping_datalen);
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/libping.c;hb=HEAD#l142
*/
static int
ping_xmit (struct ping_data *p)
{
	int i, buflen;

	if (ping_setbuf (p))
	{
		return -1;
	}

	buflen = ping_packetsize (p);

	/* Mark sequence number as sent */
	_PING_CLR (p, p->ping_num_xmit);

	/* Encode ICMP header */
	switch (p->ping_type)
	{
		case ICMP_ECHO:
			icmp_echo_encode (p->ping_buffer, buflen, p->ping_ident, p->ping_num_xmit);
			break;

		// case ICMP_TIMESTAMP:
		// 	icmp_timestamp_encode (p->ping_buffer, buflen, p->ping_ident, p->ping_num_xmit);
		// 	break;

		// case ICMP_ADDRESS:
		// 	icmp_address_encode (p->ping_buffer, buflen, p->ping_ident, p->ping_num_xmit);
		// 	break;

		default:
			icmp_generic_encode (p->ping_buffer, buflen, p->ping_type, p->ping_ident, p->ping_num_xmit);
			break;
	}

	i = sendto (
		p->ping_fd,
		(char *) p->ping_buffer,
		buflen,
		0,
		(struct sockaddr *) &p->ping_dest,
		sizeof (struct sockaddr_in)
	);
	if (i < 0)
	{
		return -1;
	}
	else
	{
		p->ping_num_xmit++;
		if (i != buflen)
		{
			printf ("ping: wrote %s %d chars, ret=%d\n", p->ping_hostname, buflen, i);
		}
	}
	return 0;
}

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping.c;hb=HEAD#l480
*/
static int
send_echo(struct ping_data *ping, t_args *args)
{
	size_t off = 0;
	int rc;

	/*
	** data_length = ping->ping_datalen
	** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping.c;hb=HEAD#l177
	** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping.c;hb=HEAD#l76
	** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping.c;hb=HEAD#l327
	** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l83
	** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/libping.c;hb=HEAD#l289
	*/
	if (PING_TIMING (args->data_length))
	{
		struct timespec now = current_timespec ();
		struct timeval tv = {
			.tv_sec = now.tv_sec,
			.tv_usec = now.tv_nsec / 1000
		};
		ping_set_data (
			ping,
			&tv,
			0,
			sizeof (tv)
		);
		off += sizeof (tv);
	}
	if (args->data_buffer)
	{
		ping_set_data (
			ping,
			args->data_buffer,
			off,
			args->data_length > off ? args->data_length - off : args->data_length
		);
	}

	if ((rc = ping_xmit (ping)) < 0)
	{
		FT_LOG_ERROR("sending packet");
	}

	return rc;
}

/*
** loop_event
** #############################################################################
*/

static bool
ping_timeout_p (struct timespec *start_time, int timeout)
{
	if (timeout == -1)
		return false;
	return timespec_sub (current_timespec (), *start_time).tv_sec >= timeout;
}

/*
** Return values
** 0: OK, continue
** 1: OK, stop
** 4: Error
*/
int loop_event(
	struct ping_data *ping,
	t_args *args,
	int fdmax,
	struct timespec *last,
	struct timespec *intvl,
	int *finishing,
	size_t *nresp
)
{
	int             ret_val;
	fd_set          fdset;
	struct timespec resp_time;
	struct timespec now;
	struct timespec *t = NULL;
	int             n;

	FD_ZERO (&fdset);
	FD_SET (ping->ping_fd, &fdset);
	now = current_timespec ();
	resp_time = timespec_sub (timespec_add (*last, *intvl), now);

	if (timespec_sign (resp_time) == -1)
	{
		resp_time.tv_sec = resp_time.tv_nsec = 0;
	}

	n = pselect (fdmax, &fdset, NULL, NULL, &resp_time, NULL);
	if (n < 0)
	{
		if (errno != EINTR)
		{
			FT_LOG_ERROR("pselect failed");
			return (4);
		}
		return (0);
	}
	else if (n == 1)
	{
		if (4 == (ret_val = ping_recv (ping, args->options)))
		{
			return (4);
		}
		if (0 == ret_val)
		{
			(*nresp)++;
		}
		if (t == 0)
		{
			now = current_timespec ();
			t = &now;
		}

		if (ping_timeout_p (&ping->ping_start_time, args->timeout))
		{
			return (1);
		}

		if (ping->ping_count && *nresp >= ping->ping_count)
		{
			return (1);
		}
	}
	else
	{
		if (!ping->ping_count || ping->ping_num_xmit < ping->ping_count)
		{
			if (send_echo (ping, args) < 0)
			{
				return (4);
			}
			if (/*!(args->options & OPT_QUIET) &&*/ args->options & OPT_FLOOD)
			{
				putchar ('.');
			}

			if (ping_timeout_p (&ping->ping_start_time, args->timeout))
			{
				return (1);
			}
		}
		else if (*finishing)
		{
			return (1);
		}
		else
		{
			*finishing = 1;

			intvl->tv_sec = args->linger;
		}
		*last = current_timespec ();
	}
	return (0);
}

void
loop_event_post(struct ping_data *ping)
{
	ping_unset_data (ping);
}

void
loop_event_pre(
	struct ping_data *ping,
	t_args *args,
	int *fdmax,
	struct timespec *last,
	struct timespec *intvl
)
{
	size_t i;

	*fdmax = ping->ping_fd + 1;

	for (i = 0; i < args->preload; i++)
		send_echo (ping, args);

	if (args->options & OPT_FLOOD)
	{
		intvl->tv_sec = 0;
		intvl->tv_nsec = 1e7;
	}
	else
		PING_SET_INTERVAL (*intvl, ping->ping_interval);

	*last = current_timespec ();
	send_echo (ping, args);
}
