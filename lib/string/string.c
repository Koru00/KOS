#include "lib/math/math.h"

int strlen(const char* str)
{
	int i;
	for (i = 0; str[i] != '\0'; i++);
	return i;
}

void strcat(char* src, const char* str)
{
	int len_src = strlen(src);
	int len_str = strlen(str);

	for (int i = (len_src); i < (len_src + len_str); i++)
	{
		src[i] = str[i - len_src];
	}

	src[len_src + len_str] = '\0';
}

char* itoa(int num)
{	
	// if the num is 0 just return "0"
	if (num == 0)
		return "0";

	int num_digits = digits(num);

	// Allocate the size of the number + '\0'
	// TODO: Use dinamyc allocation
	char* str = "";

	for (int i = 0; num != 0; i++)
	{
		// get the digit and add 48 to convert it in ascii
		char c = (char)((num % 10) + 48);

		// add c to the string	
		str[num_digits - i - 1] = c;

		// divide num by 10
		num /= 10;
	}

	// Add the null terminator to the end of the string
	str[num_digits+1] = '\0';

	// return the string
	return str;
}
