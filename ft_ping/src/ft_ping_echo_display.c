
#include "ft_memory.h"
#include "ft_ping_address.h"
#include "ft_ping_args.h"
#include "ft_ping_echo_display.h"
#include "ft_timespec.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=libicmp/icmp.h;hb=HEAD#l91
*/
#define ICMP_ECHOREPLY          0       /* Echo Reply                   */
#define ICMP_DEST_UNREACH       3       /* Destination Unreachable      */
/* Codes for ICMP_DEST_UNREACH. */
#define ICMP_NET_UNREACH        0       /* Network Unreachable          */
#define ICMP_HOST_UNREACH       1       /* Host Unreachable             */
#define ICMP_PROT_UNREACH       2       /* Protocol Unreachable         */
#define ICMP_PORT_UNREACH       3       /* Port Unreachable             */
#define ICMP_FRAG_NEEDED        4       /* Fragmentation Needed/DF set  */
#define ICMP_SR_FAILED          5       /* Source Route failed          */
#define ICMP_NET_UNKNOWN        6
#define ICMP_HOST_UNKNOWN       7
#define ICMP_HOST_ISOLATED      8
#define ICMP_NET_ANO            9
#define ICMP_HOST_ANO           10
#define ICMP_NET_UNR_TOS        11
#define ICMP_HOST_UNR_TOS       12
#define ICMP_PKT_FILTERED       13      /* Packet filtered */
#define ICMP_PREC_VIOLATION     14      /* Precedence violation */
#define ICMP_PREC_CUTOFF        15      /* Precedence cut off */
#define NR_ICMP_UNREACH         15      /* total subcodes */

#define ICMP_SOURCE_QUENCH      4       /* Source Quench                */
#define ICMP_REDIRECT           5       /* Redirect (change route)      */
/* Codes for ICMP_REDIRECT. */
#define ICMP_REDIR_NET          0       /* Redirect Net                 */
#define ICMP_REDIR_HOST         1       /* Redirect Host                */
#define ICMP_REDIR_NETTOS       2       /* Redirect Net for TOS         */
#define ICMP_REDIR_HOSTTOS      3       /* Redirect Host for TOS        */

#define ICMP_ECHO               8       /* Echo Request                 */
#define ICMP_ROUTERADV          9       /* Router Advertisement -- RFC 1256 */
#define ICMP_ROUTERDISCOVERY    10      /* Router Discovery -- RFC 1256 */
#define ICMP_TIME_EXCEEDED      11      /* Time Exceeded                */
/* Codes for TIME_EXCEEDED. */
#define ICMP_EXC_TTL            0       /* TTL count exceeded           */
#define ICMP_EXC_FRAGTIME       1       /* Fragment Reass time exceeded */

#define ICMP_PARAMETERPROB      12      /* Parameter Problem            */
#define ICMP_TIMESTAMP          13      /* Timestamp Request            */
#define ICMP_TIMESTAMPREPLY     14      /* Timestamp Reply              */
#define ICMP_INFO_REQUEST       15      /* Information Request          */
#define ICMP_INFO_REPLY         16      /* Information Reply            */
#define ICMP_ADDRESS            17      /* Address Mask Request         */
#define ICMP_ADDRESSREPLY       18      /* Address Mask Reply           */
#define NR_ICMP_TYPES           18

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l238
*/
#define NITEMS(a) sizeof(a)/sizeof((a)[0])

/*
** Forward declarations
*/
static int
print_ip_opt(unsigned int options, struct ip *ip, int hlen);
static void
print_icmp_code(int type, int code, char *prefix);
static void
print_ip_header(unsigned int options, struct ip *ip);
static void
print_ip_data(unsigned int options, struct icmp_header *icmp, void *data);
static void
print_icmp(unsigned int options, struct icmp_header *icmp, void *data);
static void
print_parameterprob(unsigned int options, struct icmp_header *icmp, void *data);

struct icmp_code_descr
{
	int type;
	int code;
	char *diag;
} icmp_code_descr[] = {
	{ICMP_DEST_UNREACH, ICMP_NET_UNREACH, "Destination Net Unreachable"},
	{ICMP_DEST_UNREACH, ICMP_HOST_UNREACH, "Destination Host Unreachable"},
	{ICMP_DEST_UNREACH, ICMP_PROT_UNREACH, "Destination Protocol Unreachable"},
	{ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, "Destination Port Unreachable"},
	{ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED, "Fragmentation needed and DF set"},
	{ICMP_DEST_UNREACH, ICMP_SR_FAILED, "Source Route Failed"},
	{ICMP_DEST_UNREACH, ICMP_NET_UNKNOWN, "Network Unknown"},
	{ICMP_DEST_UNREACH, ICMP_HOST_UNKNOWN, "Host Unknown"},
	{ICMP_DEST_UNREACH, ICMP_HOST_ISOLATED, "Host Isolated"},
	{ICMP_DEST_UNREACH, ICMP_NET_UNR_TOS, "Destination Network Unreachable At This TOS"},
	{ICMP_DEST_UNREACH, ICMP_HOST_UNR_TOS, "Destination Host Unreachable At This TOS"},
#ifdef ICMP_PKT_FILTERED
	{ICMP_DEST_UNREACH, ICMP_PKT_FILTERED, "Packet Filtered"},
#endif
#ifdef ICMP_PREC_VIOLATION
	{ICMP_DEST_UNREACH, ICMP_PREC_VIOLATION, "Precedence Violation"},
#endif
#ifdef ICMP_PREC_CUTOFF
	{ICMP_DEST_UNREACH, ICMP_PREC_CUTOFF, "Precedence Cutoff"},
#endif
	{ICMP_REDIRECT, ICMP_REDIR_NET, "Redirect Network"},
	{ICMP_REDIRECT, ICMP_REDIR_HOST, "Redirect Host"},
	{ICMP_REDIRECT, ICMP_REDIR_NETTOS, "Redirect Type of Service and Network"},
	{ICMP_REDIRECT, ICMP_REDIR_HOSTTOS, "Redirect Type of Service and Host"},
	{ICMP_TIME_EXCEEDED, ICMP_EXC_TTL, "Time to live exceeded"},
	{ICMP_TIME_EXCEEDED, ICMP_EXC_FRAGTIME, "Frag reassembly time exceeded"}
};

struct icmp_diag
{
	int type;
	char *text;
	void (*fun) (unsigned int options, struct icmp_header *, void *data);
	void *data;
} icmp_diag[] = {
	{ICMP_ECHOREPLY, "Echo Reply", NULL, NULL},
	{ICMP_DEST_UNREACH, NULL, print_icmp, "Dest Unreachable"},
	{ICMP_SOURCE_QUENCH, "Source Quench", print_ip_data, NULL},
	{ICMP_REDIRECT, NULL, print_icmp, "Redirect"},
	{ICMP_ECHO, "Echo Request", NULL, NULL},
	{ICMP_ROUTERADV, "Router Advertisement", NULL, NULL},
	{ICMP_ROUTERDISCOVERY, "Router Discovery", NULL, NULL},
	{ICMP_TIME_EXCEEDED, NULL, print_icmp, "Time exceeded"},
	{ICMP_PARAMETERPROB, NULL, print_parameterprob, NULL},
	{ICMP_TIMESTAMP, "Timestamp", NULL, NULL},
	{ICMP_TIMESTAMPREPLY, "Timestamp Reply", NULL, NULL},
	{ICMP_INFO_REQUEST, "Information Request", NULL, NULL},
	{ICMP_INFO_REPLY, "Information Reply", NULL, NULL},
	{ICMP_ADDRESS, "Address Mask Request", NULL, NULL},
	{ICMP_ADDRESSREPLY, "Address Mask Reply", NULL, NULL},
};

int
print_ip_opt(unsigned int options, struct ip *ip, int hlen)
{
	unsigned char *cp;
	int           i, j, k/*, l*/;
	// static int    old_rrlen;
	// static char   old_rr[MAX_IPOPTLEN];

	cp = (unsigned char *)(ip + 1);

	for (; hlen > (int)sizeof(struct ip); --hlen, ++cp)
	{
		switch (*cp)
		{
			// case IPOPT_EOL:
			// 	hlen = 0;
			// 	break;

			// case IPOPT_LSRR:
			// case IPOPT_SSRR:
			// 	printf("\n%cSRR: ", (*cp == IPOPT_LSRR) ? 'L' : 'S');
			// 	hlen -= 2;
			// 	j = *++cp;
			// 	++cp;
			// 	if (j > IPOPT_MINOFF)
			// 	{
			// 		for (;;)
			// 		{
			// 			/* Fetch in network byte order, calculate as host. */
			// 			l = *++cp;
			// 			l = (l << 8) + *++cp;
			// 			l = (l << 8) + *++cp;
			// 			l = (l << 8) + *++cp;
			// 			if (l == 0)
			// 			{
			// 				printf ("\t0.0.0.0");
			// 			}
			// 			else
			// 			{
			// 				struct in_addr ina;
			// 				char           *s;

			// 				ina.s_addr = htonl(l);
			// 				if (NULL == (s = sinaddr2str(options, ina)))
			// 				{
			// 					return (ERROR_OUT_OF_MEMORY);
			// 				}
			// 				printf ("\t%s", s);
			// 				free(s);
			// 			}
			// 			hlen -= 4;
			// 			j -= 4;
			// 			if (j <= IPOPT_MINOFF)
			// 			{
			// 				break;
			// 			}
			// 			putchar('\n');
			// 		}
			// 	}
			// 	break;

			// case IPOPT_RR:
			// 	j = *++cp;
			// 	i = *++cp;
			// 	hlen -= 2;
			// 	if (i > j)
			// 	{
			// 		i = j;
			// 	}
			// 	i -= IPOPT_MINOFF;
			// 	if (i <= 0)
			// 	{
			// 		break;
			// 	}
			// 	if (
			// 		i == old_rrlen
			// 		&& cp == (unsigned char *)(ip + 1) + 2
			// 		&& !memcmp((char *)cp, old_rr, i)
			// 		&& !(options & OPT_FLOOD)
			// 	)
			// 	{
			// 		printf ("\t (same route)");
			// 		i = ((i + 3) / 4) * 4;
			// 		hlen -= i;
			// 		cp += i;
			// 		break;
			// 	}
			// 	if (i < MAX_IPOPTLEN)
			// 	{
			// 		old_rrlen = i;
			// 		memmove(old_rr, cp, i);
			// 	}
			// 	else
			// 	{
			// 		old_rrlen = 0;
			// 	}

			// 	printf("\nRR: ");
			// 	j = 0;
			// 	for (;;)
			// 	{
			// 		/* Fetch in network byte order, calculate as host. */
			// 		l = *++cp;
			// 		l = (l << 8) + *++cp;
			// 		l = (l << 8) + *++cp;
			// 		l = (l << 8) + *++cp;
			// 		if (l == 0)
			// 		{
			// 			printf("\t0.0.0.0");
			// 		}
			// 		else
			// 		{
			// 			struct in_addr ina;
			// 			char           *s;

			// 			ina.s_addr = htonl(l);
			// 			if (NULL == (s = sinaddr2str(options, ina)))
			// 			{
			// 				return (ERROR_OUT_OF_MEMORY);
			// 			}
			// 			printf("\t%s", s);
			// 			free(s);
			// 		}
			// 		hlen -= 4;
			// 		i -= 4;
			// 		j += 4;
			// 		if (i <= 0)
			// 		{
			// 			break;
			// 		}
			// 		if (j >= MAX_IPOPTLEN)
			// 		{
			// 			printf("\t (truncated route)");
			// 			break;
			// 		}
			// 		putchar('\n');
			// 	}
			// 	break;

			case IPOPT_TS:
				j = *++cp;              /* len */
				i = *++cp;              /* ptr */
				hlen -= 2;
				if (i > j)
				{
					i = j;
				}

				/* Check minimal sizing. */
				if (j <= (int)(IPOPT_MINOFF + sizeof(n_time)))
				{
					break;
				}

				k = *++cp;              /* OV, FL */
				++cp;                   /* Points at first content. */
				hlen -= 2;

				printf("\nTS:");
				j = 5;                  /* First possible slot.  */
				for (;;)
				{
					char timestr[16];

					if ((k & 0x0f) != IPOPT_TS_TSONLY && ((j / 4) % 2 == 1))    /* find 5, 13, 21, 29 */
					{
						/* IP addresses */
						struct in_addr ina;
						char           *s;

						ina.s_addr = *((in_addr_t *)cp);
						if (NULL == (s = sinaddr2str(options, ina)))
						{
							return (ERROR_OUT_OF_MEMORY);
						}
						printf("\t%s", s);
						free(s);

						hlen -= sizeof(in_addr_t);
						cp += sizeof(in_addr_t);
						j += sizeof(in_addr_t);
					}
					else
					{
						/* Timestamps */
						printf(
							"\t%s ms",
							ping_cvt_time(timestr, sizeof(timestr), ntohl(*(n_time *)cp))
						);
						if (options & OPT_VERBOSE)
						{
							printf(" = 0x%08x", ntohl(*(n_time *)cp));
						}

						hlen -= sizeof (n_time);
						cp += sizeof (n_time);
						j += sizeof (n_time);

						putchar ('\n');
					}

					if (j >= i)
					{
						break;
					}
				}

				if (k & 0xf0)
				{
					printf ("\t(%u overflowing hosts)", k >> 4);
				}
				break;

			// case IPOPT_NOP:
			// 	printf ("\nNOP");
			// 	break;

			default:
				printf ("\nunknown option %x", *cp);
				break;
		} /* switch */
	} /* for */
	return (0);
}

/*
** MAYBE_UNUSED
** https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunused-parameter
** https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#index-unused-variable-attribute
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/attribute.h;hb=HEAD#l186
** Either __attribute__((unused)) or [[maybe_unused]]
*/
int
print_echo(
	unsigned int options,
	int dupflag,
	struct ping_stat *ping_stat,
	struct sockaddr_in *dest __attribute__((unused)),
	struct sockaddr_in *from,
	struct ip *ip,
	struct icmp_header *icmp,
	int datalen
)
{
	int    hlen;
	bool   timing = false;
	double triptime = 0.0;

	/* Length of IP header */
	hlen = ip->ip_hl << 2;

	/* Length of ICMP header+payload */
	datalen -= hlen;

	/* Do timing */
	if (PING_TIMING(datalen - PING_HEADER_LEN))
	{
		struct timeval  tv;
		struct timespec ts;

		timing = true;

		/* Avoid unaligned data. */
		memcpy(&tv, icmp->icmp_data, sizeof(struct timeval));
		ts = timespec_sub(
			current_timespec(),
			(struct timespec) {
				.tv_sec = tv.tv_sec,
				.tv_nsec = tv.tv_usec * 1000
			}
		);

		triptime = timespectod(ts) * 1000.0;
		ping_stat->tsum += triptime;
		ping_stat->tsumsq += triptime * triptime;
		if (triptime < ping_stat->tmin)
		{
			ping_stat->tmin = triptime;
		}
		if (triptime > ping_stat->tmax)
		{
			ping_stat->tmax = triptime;
		}
	}

	// if (options & OPT_QUIET)
	// {
	// 	return (0);
	// }
	if (options & OPT_FLOOD)
	{
		putchar('\b');
		return (0);
	}

	printf(
		"%d bytes from %s: icmp_seq=%u",
		datalen,
		inet_ntoa(*(struct in_addr *)&from->sin_addr.s_addr),
		ntohs(icmp->icmp_seq)
	);
	printf(" ttl=%d", ip->ip_ttl);
	if (timing)
	{
		printf(" time=%.3f ms", triptime);
	}
	if (dupflag)
	{
		printf(" (DUP!)");
	}

	if (ERROR_OUT_OF_MEMORY == print_ip_opt(options, ip, hlen))
	{
		return (ERROR_OUT_OF_MEMORY);
	}
	printf("\n");

	return (0);
}

void
print_icmp_code(int type, int code, char *prefix)
{
	struct icmp_code_descr *p;

	for (p = icmp_code_descr; p < icmp_code_descr + NITEMS(icmp_code_descr); p++)
	{
		if (p->type == type && p->code == code)
		{
			printf("%s\n", p->diag);
			return;
		}
	}

	printf("%s, Unknown Code: %d\n", prefix, code);
}

void
print_ip_header(unsigned int options, struct ip *ip)
{
	size_t         hlen;
	unsigned char *cp;

	hlen = ip->ip_hl << 2;
	cp = (unsigned char *)ip + sizeof(struct ip);     /* point to options */

	if (options & OPT_VERBOSE)
	{
		size_t j;

		printf("IP Hdr Dump:\n ");
		for (j = 0; j < sizeof (struct ip); ++j)
		{
			printf("%02x%s", *((unsigned char *)ip + j), (j % 2) ? " " : "");     /* Group bytes two by two. */
		}
		printf("\n");
	}

	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
	printf(" %1x  %1x  %02x", ip->ip_v, ip->ip_hl, ip->ip_tos);
	/*
	** The member `ip_len' is not portably reported in any byte order.
	** Use a simple heuristic to print a reasonable value.
	*/
	printf(
		" %04x %04x",
		(ip->ip_len > 0x2000) ? ntohs(ip->ip_len) : ip->ip_len,
		ntohs(ip->ip_id)
	);
	printf(
		"   %1x %04x",
		(ntohs(ip->ip_off) & 0xe000) >> 13,
		ntohs(ip->ip_off) & 0x1fff
	);
	printf("  %02x  %02x %04x", ip->ip_ttl, ip->ip_p, ntohs(ip->ip_sum));
	printf(" %s ", inet_ntoa(*((struct in_addr *)&ip->ip_src)));
	printf(" %s ", inet_ntoa(*((struct in_addr *)&ip->ip_dst)));
	while (hlen-- > sizeof(*ip))
	{
		printf("%02x", *cp++);
	}

	printf("\n");
}

void
print_ip_data(unsigned int options, struct icmp_header *icmp, void *data __attribute__((unused)))
{
	int        hlen;
	unsigned   char *cp;
	struct ip *ip = &icmp->icmp_ip;

	print_ip_header(options, ip);

	hlen = ip->ip_hl << 2;
	cp = (unsigned char *)ip + hlen;

	if (ip->ip_p == IPPROTO_TCP)
	{
		printf(
			"TCP: from port %u, to port %u (decimal)\n",
			(*cp * 256 + *(cp + 1)),
			(*(cp + 2) * 256 + *(cp + 3))
		);
	}
	else if (ip->ip_p == IPPROTO_UDP)
	{
		printf(
			"UDP: from port %u, to port %u (decimal)\n",
			(*cp * 256 + *(cp + 1)),
			(*(cp + 2) * 256 + *(cp + 3))
		);
	}
	else if (ip->ip_p == IPPROTO_ICMP)
	{
		int type = *cp;
		int code = *(cp + 1);

		printf(
			"ICMP: type %u, code %u, size %u",
			type,
			code,
			ntohs(ip->ip_len) - hlen
		);
		if (type == ICMP_ECHOREPLY || type == ICMP_ECHO)
		{
			printf(
				", id 0x%04x, seq 0x%04x",
				*(cp + 4) * 256 + *(cp + 5),
				*(cp + 6) * 256 + *(cp + 7)
			);
		}
		printf ("\n");
	}
}

void
print_icmp(unsigned int options, struct icmp_header *icmp, void *data)
{
	print_icmp_code(icmp->icmp_type, icmp->icmp_code, data);
	if (options & OPT_VERBOSE)
	{
		print_ip_data(options, icmp, NULL);
	}
}

void
print_parameterprob(unsigned int options, struct icmp_header *icmp, void *data)
{
	printf("Parameter problem: IP address = %s\n", inet_ntoa(icmp->icmp_gwaddr));
	print_ip_data(options, icmp, data);
}

int
print_icmp_header(
	unsigned int options,
	struct sockaddr_in *dest,
	struct sockaddr_in *from,
	struct ip *ip,
	struct icmp_header *icmp,
	int len
)
{
	int              hlen;
	struct ip        *orig_ip;
	char             *s;
	struct icmp_diag *p;

	/* Length of the IP header */
	hlen = ip->ip_hl << 2;
	/* Original IP header */
	orig_ip = &icmp->icmp_ip;

	if (
		!(
			options & OPT_VERBOSE
			|| orig_ip->ip_dst.s_addr == dest->sin_addr.s_addr
		)
	)
	{
		return (0);
	}

	if (NULL == (s = ipaddr2str(options, (struct sockaddr *)from, sizeof(struct sockaddr_in))))
	{
		return (ERROR_OUT_OF_MEMORY);
	}
	printf("%d bytes from %s: ", len - hlen, s);
	free(s);

	for (p = icmp_diag; p < icmp_diag + NITEMS(icmp_diag); p++)
	{
		if (p->type == icmp->icmp_type)
		{
			if (p->text)
			{
				printf("%s\n", p->text);
			}
			if (p->fun)
			{
				p->fun(options, icmp, p->data);
			}
			return (0);
		}
	}
	printf("Bad ICMP type: %d\n", icmp->icmp_type);
	return (0);
}
