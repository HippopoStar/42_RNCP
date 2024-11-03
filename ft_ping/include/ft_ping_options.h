
#ifndef FT_PING_OPTIONS_H
# define FT_PING_OPTIONS_H

# include <getopt.h>

/* Define keys for long options that do not have short counterparts. */
enum
{
	ARG_TTL = 256,
	ARG_IPTIMESTAMP,
};

const struct option *get_long_options(void);

#endif /* FT_PING_OPTIONS_H */
