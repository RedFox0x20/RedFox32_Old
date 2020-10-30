#include <Common/String.h>

unsigned int strlen(char *str)
{
	int n = 0;
	while (*str != 0) { n++; }
	return n;
}

