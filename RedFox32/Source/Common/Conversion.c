#include <Common/Conversion.h>
#include <Common/String.h>

unsigned int atoui(char *s)
{
	unsigned int 
		n = strlen(s),
		mul = 1,
		res = 0;

	for (int i = n-1; i >= 0; i--)
	{
		res += ((s[i] - 48) * mul);
		mul *= 10;
	}

	return res;
}

int atoi(char *s)
{
	unsigned int
		n = strlen(s),
		mul = 1;
	
	signed int res = 0;

	for (int i = n-1; i > 0; i--)
	{
		res += ((s[i] - 48) * mul);
		mul *= 10;
	}

	if (s[0] == '-')
	{
		res *= -1;
	}
	else
	{
		res += ((s[0] - 48) * mul);
	}

	return res;
}

void itoa(char *res, int val)
{
	int 
		i,
		div = 1,
		tmp = val;

	while (tmp > 0)
	{
		div *= 10;
		tmp /= 10;
	}

	for (i = 1; div != 0; i++)
	{
		res[i] = val / div;
	}
	res[i] = 0;
}

