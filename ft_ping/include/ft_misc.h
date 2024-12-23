
#ifndef FT_MISC_H
# define FT_MISC_H

# include <limits.h>
# include <stdbool.h>

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
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=Makefile;hb=HEAD#l189
** https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=m4/gnulib-common.m4;hb=HEAD#l915
*/
#define FT_CMP(n1, n2) (((n1) > (n2)) - ((n1) < (n2)))

/*
** nabs
** nsqrt
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.h;hb=HEAD#l156
** https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_common.c;hb=HEAD#l106
** Available through <math.h>: fabs, sqrt
*/
double
nabs(double a);
double
nsqrt(double a, double prec);

#endif /* FT_MISC_H */
