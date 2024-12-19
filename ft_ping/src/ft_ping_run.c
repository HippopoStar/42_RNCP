
#include "ft_log.h"
#include "ft_ping_run_loop.h"
#include "ft_ping_run.h"

#include <signal.h>
#include <unistd.h>

/*
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/File_002dScope-Variables.html
** https://www.gnu.org/software/c-intro-and-ref/manual/html_node/volatile.html
** https://www.gnu.org/software/libc/manual/html_node/Sigaction-Function-Example.html
*/
static volatile sig_atomic_t stop = 0;

static void
sig_int_handler(int signal)
{
	(void)signal;
	stop = 1;
}

static void
interruption_signal_handler_setup(struct sigaction *old_action, struct sigaction *new_action)
{
	new_action->sa_handler = sig_int_handler;
	sigemptyset(&(new_action->sa_mask));
	new_action->sa_flags = 0;

	sigaction(SIGINT, NULL, old_action);
	if (old_action->sa_handler != SIG_IGN)
	{
		stop = 0;
		sigaction(SIGINT, new_action, NULL);
	}
}

static void
interruption_signal_handler_teardown(struct sigaction *old_action)
{
	sigaction(SIGINT, old_action, NULL);
}

int
ping_run(t_args *args, struct ping_data *ping, int (*finish)(t_args *args, struct ping_data *ping))
{
	int              ret_val;
	struct sigaction old_action;
	struct sigaction new_action;

	int              fdmax;
	struct timespec  last;
	struct timespec  intvl;
	int              finishing = 0;
	size_t           nresp = 0;

	FT_LOG_DEBUG("ping_run");

	loop_event_pre(ping, args, &fdmax, &last, &intvl);

	ret_val = 0;
	interruption_signal_handler_setup(&old_action, &new_action);
	while(!(stop || ret_val))
	{
		ret_val = loop_event(
			ping,
			args,
			fdmax,
			&last,
			&intvl,
			&finishing,
			&nresp
		);
	}
	interruption_signal_handler_teardown(&old_action);

	loop_event_post(ping);

	if (finish)
		return (*finish) (args, ping);
	return (4 == ret_val ? 4 : 0);
}
