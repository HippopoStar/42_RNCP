
#include "ft_memory.h"
#include "ft_ping_args.h"
#include "ft_ping_address.h"

#include <arpa/inet.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>

char *
ipaddr2str(unsigned int options, struct sockaddr *from, socklen_t fromlen)
{
	int    err;
	size_t len;
	char   *buf, ipstr[INET6_ADDRSTRLEN], hoststr[256];

	err = getnameinfo (from, fromlen, ipstr, sizeof (ipstr), NULL, 0, NI_NUMERICHOST);
	if (err)
	{
		const char *errmsg;

		if (err == EAI_SYSTEM)
		{
			errmsg = strerror (errno);
		}
		else
		{
			errmsg = gai_strerror (err);
		}

		fprintf (stderr, "ping: getnameinfo: %s\n", errmsg);
		return (xstrdup ("unknown"));
	}

	if (options & OPT_NUMERIC)
	{
		return (xstrdup (ipstr));
	}

	err = getnameinfo (
		from,
		fromlen,
		hoststr,
		sizeof (hoststr),
		NULL,
		0,
#ifdef NI_IDN
		NI_IDN | NI_NAMEREQD
#else
		NI_NAMEREQD
#endif
	);
	if (err)
	{
		return (xstrdup (ipstr));
	}

	len = strlen (ipstr) + strlen (hoststr) + 4; /* Pair of parentheses, a space and a NUL. */
	if (NULL == (buf = xmalloc (len)))
	{
		return (NULL);
	}
	snprintf (buf, len, "%s (%s)", hoststr, ipstr);

	return (buf);
}

char *
sinaddr2str(unsigned int options, struct in_addr ina)
{
	struct hostent *hp;

	if (options & OPT_NUMERIC)
	{
		return (xstrdup (inet_ntoa (ina)));
	}

	hp = gethostbyaddr ((char *) &ina, sizeof (ina), AF_INET);
	if (hp == NULL)
	{
		return (xstrdup (inet_ntoa (ina)));
	}
	else
	{
		char *buf, *ipstr;
		int  len;

		ipstr = inet_ntoa (ina);
		len = strlen (ipstr) + 1;

		if (hp->h_name)
		{
			len += strlen (hp->h_name) + 4; /* parentheses, space, and NUL */
		}

		if (NULL == (buf = xmalloc (len)))
		{
			return (NULL);
		}
		if (hp->h_name)
		{
			snprintf (buf, len, "%s (%s)", hp->h_name, ipstr);
		}
		else
		{
			snprintf (buf, len, "%s", ipstr);
		}
		return (buf);
	}
}
