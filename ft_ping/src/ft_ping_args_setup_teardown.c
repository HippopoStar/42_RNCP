
#include "ft_log.h"
#include "ft_memory.h"
#include "ft_ping_args_setup_teardown.h"

#include <stdlib.h>

static void
free_dynamically_allocated_memory(t_args *args)
{
	if (!(NULL == args->data_buffer))
	{
		free(args->data_buffer);
	}
}

void
ping_args_teardown(t_args *args)
{
	free_dynamically_allocated_memory(args);
}

/*
** If 0 == data_length, data_buffer will be left as is
** Return values:
** 0: memory exhausted
** 1: OK (either allocation success or 0 == data_length)
*/
static int
init_data_buffer(unsigned char **data_buffer, size_t data_length, unsigned char *patptr, size_t pattern_len)
{
	size_t i;

	if (0 < data_length)
	{
		if (NULL == (*data_buffer = (unsigned char *)xmalloc(data_length * sizeof(unsigned char))))
		{
			return (0);
		}
		if (NULL == patptr)
		{
			for (i = 0; i < data_length; i++)
			{
				(*data_buffer)[i] = i;
			}
		}
		else if (0 == pattern_len)
		{
			for (i = 0; i < data_length; i++)
			{
				(*data_buffer)[i] = '\0';
			}
		}
		else
		{
			for (i = 0; i < data_length; i++)
			{
				(*data_buffer)[i] = patptr[i % pattern_len];
			}
		}
	}
	return (1);
}

int
ping_args_setup(t_args *args)
{
	return (
		(init_data_buffer(&(args->data_buffer), args->data_length, args->patptr, args->pattern_len))
			? 0
			: ERROR_MEMORY_EXHAUSTED
	);
}
