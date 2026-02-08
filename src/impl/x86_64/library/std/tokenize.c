#include "print.h"
#include "debug.h"

void tokenize(char *str)
{
    log_message(__PRETTY_FUNCTION__, "called tokenize", LOG_DEBUG);
    char token[64][100];
    int index = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] != ' ')
        {
            token[index][i] = str[i];
        }
        else
        {
            token[index][i] = '\0';
            index++;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        printf("%s\n", token[i]);
        log_message(__PRETTY_FUNCTION__, "tokes as been printed", LOG_DEBUG);
    }
    
}