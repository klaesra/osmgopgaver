#include "tests/lib.h"

int main(void)
{
    char buf[64];
    syscall_read(stdin, buf, 63);
    syscall_write(stdout, buf, 63);
    syscall_write(stdout, "\n", 1);
    
    syscall_halt();
    
    return 0;
}
