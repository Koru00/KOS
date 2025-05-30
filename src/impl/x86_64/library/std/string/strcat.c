#include "library/str/strlen.h"

void kstrcat(char* dest, const char* str)
{
	int len;
	len = strlen(dest);
	int i;
	for (i = 0; str[i] != '\0'; i++) {
		dest[i+len] = str[i];
	}
	dest[i+len] = '\0';
}
