#include "user.h"
#include "kernel/types.h"
#include "kernel/stat.h"
int pgaccess(void *start, int n, unsigned int *bitmap)
{
    return syscall(SYS_pgaccess, start, n, bitmap);
}
