#ifndef UTILS_H
#define UTILS_H

int max(int a, int b);
int min(int a, int b);
bool strequal(char *a, char *b);

#ifdef NDEBUG
# define assert(EX)
#else
# define assert(EX) (void)((EX) || (__assert (#EX, __FILE__, __LINE__, __func__),0))
#endif

void __assert(const char *msg, const char *file, int line, const char *func);

#endif // UTILS_H
