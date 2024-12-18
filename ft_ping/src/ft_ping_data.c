
# include "ft_ping_data.h"

void
ping_set_type(struct ping_data *ping, int type)
{
	ping->ping_type = type;
}

void
ping_set_event_handler(struct ping_data *ping, ping_efp pf, void *closure)
{
	ping->ping_event = pf;
	ping->ping_closure = closure;
}

void
ping_set_packetsize(struct ping_data *ping, size_t size)
{
	ping->ping_datalen = size;
}

int
ping_set_dest(struct ping_data *ping, const char *host)
{
#if HAVE_DECL_GETADDRINFO
	FT_LOG_DEBUG("HAVE_DECL_GETADDRINFO");
	int             rc;
	struct addrinfo hints, *res;
	char            *rhost;

# if defined HAVE_IDN || defined HAVE_IDN2
	FT_LOG_DEBUG("HAVE_IDN || HAVE_IDN2");
	rc = idna_to_ascii_lz(host, &rhost, 0); /* RHOST is allocated. */
	if (rc)
	{
		return (1);
	}
	host = rhost;
# else
	rhost = NULL;
# endif

	memset(&hints, '\0', sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_CANONNAME;
# ifdef AI_IDN
	FT_LOG_DEBUG("AI_IDN");
	hints.ai_flags |= AI_IDN;
# endif
# ifdef AI_CANONIDN
	FT_LOG_DEBUG("AI_CANONIDN");
	hints.ai_flags |= AI_CANONIDN;
# endif

	rc = getaddrinfo(host, NULL, &hints, &res);

	if (rc)
	{
		free(rhost);
		return (1);
	}

	memcpy(&(ping->ping_dest), res->ai_addr, res->ai_addrlen);
	if (res->ai_canonname)
	{
		ping->ping_hostname = strdup(res->ai_canonname);
	}
	else
	{
# if defined HAVE_IDN || defined HAVE_IDN2
		ping->ping_hostname = host;
# else
		ping->ping_hostname = strdup(host);
# endif
	}

	freeaddrinfo(res);

	return (0);
#else /* !HAVE_DECL_GETADDRINFO */

	struct sockaddr_in *s_in = &(ping->ping_dest);
	s_in->sin_family = AF_INET;
# ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
	FT_LOG_DEBUG("HAVE_STRUCT_SOCKADDR_IN_SIN_LEN");
	s_in->sin_len = sizeof(struct sockaddr_in);
# endif
	if (inet_aton(host, &s_in->sin_addr))
	{
		ping->ping_hostname = strdup(host);
	}
	else
	{
		struct hostent *hp;
# if defined HAVE_IDN || defined HAVE_IDN2
		char *rhost;
		int rc;

		rc = idna_to_ascii_lz(host, &rhost, 0);
		if (rc)
		{
			return (1);
		}
		hp = gethostbyname(rhost);
		free (rhost);
# else/* !HAVE_IDN && !HAVE_IDN2 */
		hp = gethostbyname(host);
# endif
		if (!hp)
		{
			return (1);
		}

		s_in->sin_family = hp->h_addrtype;
		if (hp->h_length > (int)sizeof(s_in->sin_addr))
		{
			hp->h_length = sizeof(s_in->sin_addr);
		}

		memcpy(&s_in->sin_addr, hp->h_addr, hp->h_length);
		ping->ping_hostname = strdup(hp->h_name);
	}
	return (0);
#endif /* !HAVE_DECL_GETADDRINFO */
}

double
nabs(double a)
{
	return ((a < 0) ? -a : a);
}

double
nsqrt(double a, double prec)
{
	double x0, x1;

	if (a < 0)
	{
		return (0);
	}
	if (a < prec)
	{
		return (0);
	}
	x1 = a / 2;
	do
	{
		x0 = x1;
		x1 = (x0 + a / x0) / 2;
	}
	while (nabs(x1 - x0) > prec);

	return (x1);
}
