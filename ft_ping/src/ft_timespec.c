
#include "ft_timespec.h"

#include <sys/time.h>

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

/*
** TYPE_SIGNED
** TYPE_WIDTH
** TYPE_MINIMUM
** TYPE_MAXIMUM
** https://sourceware.org/git/?p=glibc.git;a=blob;f=include/intprops.h;hb=HEAD#l39
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/intprops.h;hb=HEAD#l30
** Minimum and maximum values for integer types and expressions.
*/
#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))
#define TYPE_WIDTH(t) (sizeof (t) * CHAR_BIT)
#define TYPE_MINIMUM(t) ((t) ~ TYPE_MAXIMUM (t))
#define TYPE_MAXIMUM(t) \
	((t) (! TYPE_SIGNED (t) \
	? (t) -1 \
	: ((((t) 1 << (TYPE_WIDTH (t) - 2)) - 1) * 2 + 1)))

/*
** ckd_add
** ckd_sub
** ckd_mul
** https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
**
** https://sourceware.org/git/?p=glibc.git;a=blob;f=include/intprops.h;hb=HEAD#l389
**
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/stdckdint.in.h;hb=HEAD
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/intprops-internal.h;hb=HEAD#l147
*/
#define ckd_add(r, a, b) __builtin_add_overflow(a, b, r)
#define ckd_sub(r, a, b) __builtin_sub_overflow(a, b, r)
#define ckd_mul(r, a, b) __builtin_mul_overflow(a, b, r)

/*
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/timespec.h;hb=HEAD#l84
** Return an approximation to A, of type 'double'.
*/
double
timespectod(struct timespec a)
{
	return (a.tv_sec + a.tv_nsec / 1e9);
}

/*
** make_timespec
** https://sourceware.org/git/?p=glibc.git;a=blob;f=support/timespec.h;hb=HEAD#l27
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/timespec.h;hb=HEAD#l53
*/
struct timespec
make_timespec(time_t s, long int ns)
{
	return ((struct timespec) { .tv_sec = s, .tv_nsec = ns });
}

/*
** https://sourceware.org/git/?p=glibc.git;a=blob;f=support/timespec.h;hb=HEAD#l39
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/timespec.h;hb=HEAD#l77
**
** timespec_add
** https://sourceware.org/git/?p=glibc.git;a=blob;f=support/timespec-add.c;hb=HEAD#l27
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/timespec-add.c;hb=HEAD#l29
**
** timespec_sub
** https://sourceware.org/git/?p=glibc.git;a=blob;f=support/timespec-sub.c;hb=HEAD#l28
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/timespec-sub.c;hb=HEAD#l30
*/
struct timespec
timespec_sub (struct timespec a, struct timespec b)
{
	int    nsdiff = a.tv_nsec - b.tv_nsec;
	int    borrow = nsdiff < 0;
	time_t rs;
	int    rns;
	bool   v = ckd_sub (&rs, a.tv_sec, b.tv_sec);
	if (v == ckd_sub (&rs, rs, borrow))
	{
		rns = nsdiff + TIMESPEC_HZ * borrow;
	}
	else
	{
		if ((TYPE_MINIMUM (time_t) + TYPE_MAXIMUM (time_t)) / 2 < rs)
		{
			rs = TYPE_MINIMUM (time_t);
			rns = 0;
		}
		else
		{
			rs = TYPE_MAXIMUM (time_t);
			rns = TIMESPEC_HZ - 1;
		}
	}

	return (make_timespec (rs, rns));
}

/*
** timespec_get
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/timespec_get.c;hb=HEAD
*/

/* Get the system time into *TS. */
static void
gettime(struct timespec *ts)
{
#if defined CLOCK_REALTIME && HAVE_CLOCK_GETTIME
	clock_gettime(CLOCK_REALTIME, ts);
#elif defined HAVE_TIMESPEC_GET
	timespec_get(ts, TIME_UTC);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*ts = (struct timespec)
		{
			.tv_sec  = tv.tv_sec,
			.tv_nsec = tv.tv_usec * 1000
		};
#endif
}

/*
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/timespec.h;hb=HEAD#l92
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/gettime.c;hb=HEAD#l43
** Return the current system time as a struct timespec.
*/
struct timespec
current_timespec(void)
{
	struct timespec ts;
	gettime(&ts);
	return (ts);
}

/*
** is_normed_time
** ping_cvt_time
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.c;hb=HEAD#l132
*/
static int
is_normed_time(n_time t)
{
	/* A set MSB indicates non-normalised time standard.  */
	return ((t & (1UL << 31)) ? 0 : 1);
}

const char *
ping_cvt_time(char *buf, size_t buflen, n_time t)
{
	n_time t_red;

	t_red = t & ((1UL << 31) - 1);

	if (is_normed_time(t))
	{
		snprintf(buf, buflen, "%u", t_red);
	}
	else
	{
		snprintf(buf, buflen, "<%u>", t_red);
	}

	return (buf);
}
