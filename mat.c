int mod (int dividend, int divisor)
{
	while (dividend >= divisor)
		dividend = dividend - divisor;
	return dividend;
}

int div (int dividend, int divisor)
{
	int q = 0;
	while (q * divisor <= dividend)
		q = q + 1;
	return q-1;
}