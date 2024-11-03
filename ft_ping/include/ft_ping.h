
#ifndef FT_PING_H
# define FT_PING_H

# include <limits.h>
# include <stddef.h>

# define MAXWAIT          10                     /* Max seconds to wait for response. */
# define MAXPATTERN       16                     /* Maximal length of pattern. */
# define PING_HEADER_LEN  8                      /* ICMP_MINLEN (IPv4) */
# define PING_DATALEN     (64 - PING_HEADER_LEN) /* default data length */
# define MAXIPLEN         60
# define MAXICMPLEN       76
# define PING_MAX_DATALEN (65535 - MAXIPLEN - MAXICMPLEN)

# define OPT_FLOOD       0x001
# define OPT_NUMERIC     0x004
# define OPT_VERBOSE     0x020
# define OPT_IPTIMESTAMP 0x040
# define SOPT_TSONLY     0x001
# define SOPT_TSADDR     0x002

typedef struct    s_args
{
	unsigned long preload;             /* default value: 0           ; max value: INT_MAX */
	int           timeout;             /* default value: -1          ; max value: INT_MAX */
	int           linger;              /* default value: MAXWAIT     ; max value: INT_MAX */
	int           pattern_len;         /* default value: MAXPATTERN  ; */
	unsigned char pattern[MAXPATTERN];
	size_t        data_length;         /* default value: PING_DATALEN; max value: PING_MAX_DATALEN */
	int           tos;                 /* default value: -1          ; max value: 255 */
	int           ttl;                 /* default value: 0           ; max value: 255 */
	int           socket_type;         /* https://sourceware.org/git/?p=glibc.git;a=blob;f=bits/socket.h;hb=HEAD#l337 */
	unsigned int  options;
	unsigned int  suboptions;
}                 t_args;

#endif /* FT_PING_H */
