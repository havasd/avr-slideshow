#include "utils.h"

int strlen(const char *string)
{
    int len = 0;

    char c = string[0];
    while (c)
        c = string[++len];

    return len;
}
