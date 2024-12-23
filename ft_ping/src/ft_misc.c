
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
