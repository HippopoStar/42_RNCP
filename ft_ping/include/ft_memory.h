
#ifndef FT_MEMORY_H
# define FT_MEMORY_H

# include <string.h>

/*
** https://sourceware.org/git/?p=glibc.git;a=blob;f=locale/programs/xmalloc.c;hb=HEAD#l46
*/
# define ERROR_MEMORY_EXHAUSTED 4

void *
xmalloc(size_t n);

void *
xstrdup(char *string);

#endif /* FT_MEMORY_H */
