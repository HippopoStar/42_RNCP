
#ifndef FT_MEMORY_H
# define FT_MEMORY_H

# include <string.h>

# define ERROR_OUT_OF_MEMORY 4

void *
xmalloc(size_t n);

void *
xstrdup(char *string);

#endif /* FT_MEMORY_H */
