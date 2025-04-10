#include "user.h"
#include "kernel/types.h"
#include "kernel/stat.h"
int pgaccess(void *start, int n, unsigned int *bitmap)
{
    for (int i = 0; i < n; i++) {
        if (/* điều kiện kiểm tra access */) {
            bitmap[i / 64] |= (1ULL << (i % 64));  // Sử dụng uint64 * để ghi kết quả
        }
    }
    
    return 0;  // Thành công
}
