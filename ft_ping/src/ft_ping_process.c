
#include "ft_ping_args.h"
#include "ft_ping_data_setup_teardown.h"
#include "ft_ping_echo.h"

#include <unistd.h>

void
ping_process(t_args *args, int argc, char **argv)
{
	(void)args;
	(void)argc;
	(void)argv;

	struct ping_data p;
	int              i;
	int              status;

	status = 0;
	if (ping_data_setup(&p))
	{
		i = 0;
		/*
		** main return statement: https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping.c;hb=HEAD#l327
		** ping_echo return statement: https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l150
		** ping_run return statement: https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping.c;hb=HEAD#l475
		** echo_finish return statement: https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=blob;f=ping/ping_echo.c;hb=HEAD#l640
		**
		** ping_echo return values
		** 0: nominal case
		** 1: at least one of target hosts was unreachable
		** 2: error
		*/
		while (i < argc && (status |= ping_echo(args, &p, argv[i])) < 2)
		{
			i++;
		}
	}
	ping_data_teardown(&p);
}
