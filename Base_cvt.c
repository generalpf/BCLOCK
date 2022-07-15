#include <stdlib.h>
#include "bclock.h"

int max_place_array[15] = { 8, 6, 4, 4, 4, 3, 3, 3, 2, 3, 3, 3, 3, 3, 2 };
// The above line starts at 2 and ends at 16
char set[] = "0123456789ABCDEF";

int power(int base, int exponent)
{
	if (!exponent)
		return 1;
	else
		return base * (power(base,exponent - 1));
}

char * base_cvt(int base, int number)
{
	char *temp, index = 0;
	int count;
	int max_places = max_place_array[base - 2];
	temp = (char *) malloc((max_places + 1) * sizeof(char));
	for (count = max_places; count; count--) {
		temp[index++] = set[number / power(base,count - 1)];
		number %= power(base,count - 1);
	}
	temp[index] = '\0';
	return temp;
}
