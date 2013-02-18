#include "tests/lib.h"

int main(void)
{
    char buf[64]; // create char buffer of 64 bytes
    syscall_write(stdout, "Press the any-key!\n", 19); // we try to write stuff! Max length = 19 chars
    syscall_read(stdin, buf, 63); // read user input
    syscall_write(stdout, "You pressed: ", 13); // write stuff again max length = 13 chars
    syscall_write(stdout, buf, 63); // write the char clicked by the user
    syscall_write(stdout, "\n", 1); // newline!
    syscall_write(stdout, "Press the OTHER any-key!\n", 25); // we test again.
    syscall_read(stdin, buf, 63); // read user input again to test hypothesis
    syscall_write(stdout, "You pressed: ", 13); // write stuff again max length = 13 chars
    syscall_write(stdout, buf, 63); // aaaaand write it!
    syscall_write(stdout, "\n", 1); // newline!
    
    syscall_halt(); // halt the system when testing is done.
    
    return 0;
}
