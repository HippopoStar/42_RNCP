
#include "ft_log.h"

#include <stdlib.h>
#include <string.h>

/*
** xmalloc
** xstrdup
** oom_error
** https://sourceware.org/git/?p=glibc.git;a=blob;f=include/programs/xmalloc.h;hb=HEAD
** https://sourceware.org/git/?p=glibc.git;a=blob;f=locale/programs/xmalloc.c;hb=HEAD
** https://sourceware.org/git/?p=glibc.git;a=blob;f=locale/programs/xstrdup.c;hb=HEAD
** https://sourceware.org/git/?p=glibc.git;a=blob;f=support/support.h;hb=HEAD#l99
** https://sourceware.org/git/?p=glibc.git;a=blob;f=support/xmalloc.c;hb=HEAD
** https://sourceware.org/git/?p=glibc.git;a=blob;f=support/xstrdup.c;hb=HEAD
** https://sourceware.org/git/?p=glibc.git;a=blob;f=support/oom_error.c;hb=HEAD
*/

static void *
fixup_null_alloc(size_t n)
{
	void *p;

	p = NULL;
	if (n == 0)
	{
		p = malloc ((size_t) 1);
	}
	if (p == NULL)
	{
		// error (xmalloc_exit_failure, 0, _("memory exhausted"));
		FT_LOG_ERROR("memory exhausted");
	}
	return p;
}

/* Allocate N bytes of memory dynamically, with error checking.  */
void *
xmalloc(size_t n)
{
	void *p;

	p = malloc (n);
	if (p == NULL)
	{
		p = fixup_null_alloc (n);
	}
	return p;
}

/* Return a newly allocated copy of STRING.  */
void *
xstrdup(char *string)
{
	return strcpy (xmalloc (strlen (string) + 1), string);
}
