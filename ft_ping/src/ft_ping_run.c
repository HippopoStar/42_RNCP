
#include "ft_ping_run.h"

#include <signal.h>
#include <stdio.h>
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

void
ping_run(t_args *args)
{
	(void)args;

	struct sigaction old_action;
	struct sigaction new_action;

	new_action.sa_handler = sig_int_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;

	sigaction(SIGINT, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
	{
		sigaction (SIGINT, &new_action, NULL);
	}

	while(!stop)
	{
		write(1, ".", 1);
		sleep(1);
	}
	printf("\n");
}
