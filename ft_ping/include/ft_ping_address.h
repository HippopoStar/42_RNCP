
#ifndef FT_PING_ADDRESS_H
# define FT_PING_ADDRESS_H

# include <netdb.h>

char *
ipaddr2str(unsigned int options, struct sockaddr *from, socklen_t fromlen);

char *
sinaddr2str(unsigned int options, struct in_addr ina);

#endif /* FT_PING_ADDRESS_H */
