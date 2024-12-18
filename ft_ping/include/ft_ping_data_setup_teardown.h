
#ifndef FT_PING_DATA_SETUP_TEARDOWN_H
# define FT_PING_DATA_SETUP_TEARDOWN_H

# include "ft_ping_args.h"
# include "ft_ping_data.h"

int
ping_data_setup(struct ping_data *p, t_args *args, int *one);

void
ping_data_teardown(struct ping_data *p);

#endif /* FT_PING_DATA_SETUP_TEARDOWN_H */
