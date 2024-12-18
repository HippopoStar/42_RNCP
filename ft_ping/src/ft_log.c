
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const char *program_name = NULL;

static void set_program_name(const char *argv0)
{
	program_name = argv0;
}

void
ft_log_init(const char *argv0)
{
	set_program_name(argv0);
	/* Force line buffering regardless of output device. */
	setvbuf (stdout, NULL, _IOLBF, 0);
}

int
ft_log_debug(const char *format, ...)
{
	int     ret_val;
	va_list ap;

	(void)format;
#ifdef DEBUG
	va_start(ap, format);
	ret_val = vfprintf(stderr, format, ap);
	va_end(ap);
#else
	ret_val = 0;
#endif
	return (ret_val);
}

int
ft_log_info(const char *format, ...)
{
	int     ret_val;
	va_list ap;

	va_start(ap, format);
	ret_val = vfprintf(stdout, format, ap);
	va_end(ap);
	return (ret_val);
}

/*
** https://sourceware.org/git/?p=glibc.git;a=blob;f=argp/argp.h;hb=HEAD#l467
** https://sourceware.org/git/?p=glibc.git;a=blob;f=argp/argp-help.c;hb=HEAD#l1862
**
** https://sourceware.org/git/?p=glibc.git;a=blob;f=misc/error.h;hb=HEAD#l27
** https://sourceware.org/git/?p=glibc.git;a=blob;f=misc/error.c;hb=HEAD#l230
*/
int
ft_log_error(const char *format, ...)
{
	int     ret_val;
	va_list ap;

	va_start(ap, format);
	ret_val = vfprintf(stderr, format, ap);
	va_end(ap);
	return (ret_val);
}
