#ifndef UTILS_H
#define UTILS_H

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#ifdef NO_STDLIB
int strlen(const char *message);
#else
#include <string.h>
#endif

#endif // UTILS_H

