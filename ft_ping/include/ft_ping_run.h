#ifndef FT_PING_RUN_H
# define FT_PING_RUN_H

# include "ft_ping_args.h"
# include "ft_ping_data.h"

int
ping_run(t_args *args, struct ping_data *ping, int (*finish)(t_args *args, struct ping_data *ping));

#endif /* FT_PING_RUN_H */
