
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
	(void)args;
	(void)ping;
	(void)finish;

	struct sigaction old_action;
	struct sigaction new_action;

	interruption_signal_handler_setup(&old_action, &new_action);
	while(!stop)
	{
		write(1, ".", 1);
		sleep(1);
	}
	write(1, "\n", 1);
	interruption_signal_handler_teardown(&old_action);
	return (0);
}
