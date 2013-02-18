#include "tests/lib.h"
#include "proc/syscall.h"

int main(void)
{
    syscall_read(FILEHANDLE_STDIN, (void *)FILEHANDLE_STDOUT, 10);
    syscall_write(FILEHANDLE_STDOUT, (const void *)FILEHANDLE_STDIN, 10);

    return 0;
}
