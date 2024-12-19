
#ifndef FT_PING_RUN_LOOP_H
# define FT_PING_RUN_LOOP_H

# include "ft_ping_args.h"
# include "ft_ping_data.h"
# include "ft_timespec.h"

# include <string.h>

int loop_event(
	struct ping_data *ping,
	t_args *args,
	int fdmax,
	struct timespec *last,
	struct timespec *intvl,
	int *finishing,
	size_t *nresp
);

void
loop_event_post(struct ping_data *ping);

void
loop_event_pre(
	struct ping_data *ping,
	t_args *args,
	int *fdmax,
	struct timespec *last,
	struct timespec *intvl
);

#endif /* FT_PING_RUN_LOOP_H */
