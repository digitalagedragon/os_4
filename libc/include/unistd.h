#ifndef UNISTD_H
#define UNISTD_h

#include <sys/types/pid_t.h>

pid_t fork(void);
int execve(const char *filename, char *const argv[], char *const envp[]);
int brk(void *addr);
void *sbrk(int inc);
#endif
