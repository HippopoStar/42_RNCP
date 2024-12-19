
#ifndef FT_TIMESPEC_H
# define FT_TIMESPEC_H

# include <stdint.h>
# include <string.h>
# include <time.h>

/*
** TIMESPEC_HZ
** https://sourceware.org/git/?p=glibc.git;a=blob;f=support/timespec.h;hb=HEAD#l36
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/timespec.h;hb=HEAD#l44
*/
enum { TIMESPEC_HZ = 1000000000 };

/*
** n_time
** https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/generic/netinet/in_systm.h;hb=HEAD
** ms since 00:00 GMT, byte rev
*/
typedef uint32_t n_time;

double
timespectod(struct timespec a);

struct timespec
make_timespec(time_t s, long int ns);

int
timespec_sign(struct timespec a);

struct timespec
timespec_add(struct timespec a, struct timespec b);

struct timespec
timespec_sub(struct timespec a, struct timespec b);

struct timespec
current_timespec(void);

const char *
ping_cvt_time(char *buf, size_t buflen, n_time t);

#endif /* FT_TIMESPEC_H */
