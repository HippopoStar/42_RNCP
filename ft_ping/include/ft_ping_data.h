
#ifndef FT_PING_DATA_H
# define FT_PING_DATA_H

# include "ft_timespec.h"

# include <arpa/inet.h>

# include <netdb.h>
# include <string.h>

/*
** ICMP_ECHO
** https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/gnu/netinet/ip_icmp.h;hb=HEAD#l51
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=libicmp/icmp.h;hb=HEAD#l120
** Also defined in ft_ping_icmp.h
*/
# define ICMP_ECHO             8    /* Echo Request */

/*
** IPOPT_TS_TSONLY
** IPOPT_TS_TSANDADDR
** https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/generic/netinet/ip.h;hb=HEAD#l286
** flag bits for ipt_flg
*/
# define IPOPT_TS_TSONLY       0    /* timestamps only */
# define IPOPT_TS_TSANDADDR    1    /* timestamps and addresses */

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.h;hb=HEAD#l54
*/
# define PEV_RESPONSE          0
# define PEV_DUPLICATE         1
# define PEV_NOECHO            2
# define PING_CKTABSIZE        128
# define PING_TIMING(s)        ((s) >= (int)sizeof(struct timeval))
# define PING_DEFAULT_INTERVAL 1000 /* Milliseconds */

/*
** struct in_addr
** https://sourceware.org/git/?p=glibc.git;a=blob;f=inet/netinet/in.h;hb=HEAD#l29
** Internet address.
** Included through <netdb.h>
*/

/*
** struct sockaddr_in
** https://sourceware.org/git/?p=glibc.git;a=blob;f=inet/netinet/in.h;hb=HEAD#l248
** Structure describing an Internet socket address.
** Included through <netdb.h>
*/

/*
** struct ip
** https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Bit-Fields.html
** https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/generic/netinet/ip.h;hb=HEAD#l104
** Structure of an internet header, naked of options.
*/
struct ip
{
# if __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int   ip_hl:4;        /* header length */
	unsigned int   ip_v:4;         /* version */
# endif
# if __BYTE_ORDER == __BIG_ENDIAN
	unsigned int   ip_v:4;         /* version */
	unsigned int   ip_hl:4;        /* header length */
# endif
	uint8_t        ip_tos;         /* type of service */
	unsigned short ip_len;         /* total length */
	unsigned short ip_id;          /* identification */
	unsigned short ip_off;         /* fragment offset field */
# define IP_RF      0x8000         /* reserved fragment flag */
# define IP_DF      0x4000         /* dont fragment flag */
# define IP_MF      0x2000         /* more fragments flag */
# define IP_OFFMASK 0x1fff         /* mask for fragmenting bits */
	uint8_t        ip_ttl;         /* time to live */
	uint8_t        ip_p;           /* protocol */
	unsigned short ip_sum;         /* checksum */
	struct in_addr ip_src, ip_dst; /* source and dest address */
};

/*
** icmp_header
** https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/gnu/netinet/ip_icmp.h;hb=HEAD#l26
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=libicmp/icmp.h;hb=HEAD#l19
*/
struct icmp_header
{
	unsigned char          icmp_type;  /* type of message, see below */
	unsigned char          icmp_code;  /* type sub code */
	unsigned short         icmp_cksum; /* ones complement cksum of struct */
	union
	{
		unsigned char      ih_pptr;   /* ICMP_PARAMPROB */
		struct in_addr     ih_gwaddr; /* ICMP_REDIRECT */
		struct ih_idseq
		{
			unsigned short icd_id;
			unsigned short icd_seq;
		}                  ih_idseq;
		int                ih_void;

		/* ICMP_UNREACH_NEEDFRAG -- Path MTU discovery as per rfc 1191 */
		struct ih_pmtu
		{
			unsigned short ipm_void;
			unsigned short ipm_nextmtu;
		}                  ih_pmtu;

		/* ICMP_ROUTERADV -- RFC 1256 */
		struct ih_rtradv
		{
			unsigned char  irt_num_addrs; /* Number of addresses following the msg */
			unsigned char  irt_wpa;       /* Address Entry Size (32-bit words) */
			unsigned short irt_lifetime;  /* Lifetime */
		}                  ih_rtradv;

	}                      icmp_hun;
# define icmp_pptr      icmp_hun.ih_pptr
# define icmp_gwaddr    icmp_hun.ih_gwaddr
# define icmp_id        icmp_hun.ih_idseq.icd_id
# define icmp_seq       icmp_hun.ih_idseq.icd_seq
# define icmp_void      icmp_hun.ih_void
# define icmp_pmvoid    icmp_hun.ih_pmtu.ipm_void
# define icmp_nextmtu   icmp_hun.ih_pmtu.ipm_nextmtu
# define icmp_num_addrs icmp_hun.ih_rtradv.irt_num_addrs
# define icmp_wpa       icmp_hun.ih_rtradv.irt_wpa
# define icmp_lifetime  icmp_hun.ih_rtradv.irt_lifetime

	union
	{
		struct id_ts /* ICMP_TIMESTAMP, ICMP_TIMESTAMPREPLY */
		{
			n_time         its_otime; /* Originate timestamp */
			n_time         its_rtime; /* Recieve timestamp */
			n_time         its_ttime; /* Transmit timestamp */
		}                  id_ts;
		struct id_ip /* Original IP header */
		{
			struct ip      idi_ip;
			/* options and then 64 bits of data */
		}                  id_ip;
		unsigned long      id_mask; /* ICMP_ADDRESS, ICMP_ADDRESSREPLY */
		char               id_data[1];
	}                      icmp_dun;
# define icmp_otime     icmp_dun.id_ts.its_otime
# define icmp_rtime     icmp_dun.id_ts.its_rtime
# define icmp_ttime     icmp_dun.id_ts.its_ttime
# define icmp_ip        icmp_dun.id_ip.idi_ip
# define icmp_mask      icmp_dun.id_mask
# define icmp_data      icmp_dun.id_data
};

/*
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Floating_002dPoint-Data-Types.html
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.h;hb=HEAD#l46
*/
struct ping_stat
{
	double tmin;   /* minimum round trip time */
	double tmax;   /* maximum round trip time */
	double tsum;   /* sum of all times, for doing average */
	double tsumsq; /* sum of all times squared, for std. dev. */
};

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.h;hb=HEAD#l87
*/
typedef int (*ping_efp) (
	unsigned int       options,
	int                code,
	void               *closure,
	struct sockaddr_in *dest,
	struct sockaddr_in *from,
	struct ip          *ip,
	struct icmp_header *icmp,
	int                datalen
);

/*
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.h;hb=HEAD#l105
*/
struct ping_data
{
	int                ping_fd;         /* Raw socket descriptor */
	int                ping_type;       /* Type of packets to send */
	size_t             ping_count;      /* Number of packets to send */
	struct timespec    ping_start_time; /* Start time */
	size_t             ping_interval;   /* Number of seconds to wait between sending pkts */
	struct sockaddr_in ping_dest;       /* whom to ping */
	char               *ping_hostname;  /* Printable hostname */
	size_t             ping_datalen;    /* Length of data */
	int                ping_ident;      /* Our identifier */
	ping_efp           ping_event;      /* User-defined handler */
	void               *ping_closure;   /* User-defined data */

	/* Runtime info */
	int                ping_cktab_size;
	char               *ping_cktab;

	unsigned char      *ping_buffer;    /* I/O buffer */
	struct sockaddr_in ping_from;
	size_t             ping_num_xmit;   /* Number of packets transmitted */
	size_t             ping_num_recv;   /* Number of packets received */
	size_t             ping_num_rept;   /* Number of duplicates received */
};

void
ping_unset_data(struct ping_data *p);

int
ping_setbuf(struct ping_data *p);

int
ping_set_data(struct ping_data *p, void *data, size_t off, size_t len);

void
ping_set_count(struct ping_data *ping, size_t count);

int
ping_set_sockopt(struct ping_data *ping, int opt, void *val, int valsize);

int
ping_set_socket_fd(void);

void
ping_set_type(struct ping_data *ping, int type);

void
ping_set_event_handler(struct ping_data *ping, ping_efp pf, void *closure);

void
ping_set_packetsize(struct ping_data *ping, size_t size);

int
ping_set_dest(struct ping_data *ping, const char *host);

int
ping_init(struct ping_data *p, int type, int ident);

/*
** https://sourceware.org/git/?p=glibc.git;a=blob;f=inet/arpa/inet.h;hb=HEAD
**
** inet_ntoa
** https://sourceware.org/git/?p=glibc.git;a=blob;f=inet/inet_ntoa.c;hb=HEAD
** Convert Internet number in IN to ASCII representation. The return value
** is a pointer to an internal array containing the string.
**
** inet_aton
** https://sourceware.org/git/?p=glibc.git;a=blob;f=resolv/inet_addr.c;hb=HEAD#l197
** Convert Internet host address from numbers-and-dots notation in CP
** into binary data and store the result in the structure INP.
**
** Included through <arpa/inet.h>
*/

#endif /* FT_PING_DATA_H */
