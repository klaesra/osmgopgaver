/*
 * System calls.
 *
 * Copyright (C) 2003 Juha Aatrokoski, Timo Lilja,
 *   Leena Salmela, Teemu Takanen, Aleksi Virtanen.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: syscall.c,v 1.3 2004/01/13 11:10:05 ttakanen Exp $
 *
 */
#include "kernel/cswitch.h"
#include "proc/syscall.h"
#include "proc/process.h"
#include "kernel/halt.h"
#include "kernel/panic.h"
#include "lib/libc.h"
#include "kernel/assert.h"
#include "drivers/device.h"
#include "drivers/gcd.h"
#include "fs/vfs.h"

/* A call to syscall_open prepares the file indicated by pathname for
   reading and writing (read- only mode is not supported). The
   pathname includes a volume name and a file name (for instance
   [root]a.out). Returns a positive number bigger than 2 (serving as
   the file handle to the open file) when successful, or a negative
   number on error. */
int syscall_open(char *pathname){
    int ret = (int)vfs_open(pathname);
    if (ret < 0)
        return ret; // Error opening file, return negative value
    else
        return ret + 3; // return filehandle greather than 2
}

/* This system call has the effect to invalidate the file handle
   passed as a parameter. It cannot be used in file operations any
   more afterwards. */
int syscall_close(int filehandle) {
    return vfs_close((openfile_t)filehandle); //close file and return
}

/* Creates a new file with name pathname of size size. Returns 0 on
   success, or a negative value on error. */
int syscall_create(char* pathname, int size) {
    return vfs_create(pathname, size);
}

/* Deletes the file with name pathname from the file system, fails if
   the file is open. Returns 0 on success, or a negative number on
   error. */
int syscall_delete(char *pathname) {
    return vfs_remove(pathname);
}

/* Set the reading or writing position of the open file represented by
   filehandle to the indicated absolute offset (in bytes from the
   beginning). Returns 0 on success, a negative number on
   error. Seeking beyond the end of the file sets the position to the
   end and produces an error return value. */
int syscall_seek(int filehandle, int offset){
    return vfs_seek((openfile_t)filehandle, offset);
}

/* Reads at most length bytes from the file identified by filehandle
   (at the current file position) into buffer, advancing the file
   position. Returns the number of bytes actually read (before
   reaching the end of the file), or a negative value when an error
   occurred. */
//int syscall_read(int filehandle, void *buffer, int length) {
//}

/* Writes length bytes from buffer to the open file identified by
   filehandle, starting at the current position and advancing the
   position. Returns the number of bytes actually written, or a
   negative value on error. */
//int syscall_write(int filehandle, const void *buffer, int length){
//}


/* returns the number of bytes read before reaching the EOF 
 * or a negative value on error 
 */
int syscall_read(int fhandle, void *buffer, int length) {
    fhandle = fhandle;
    gcd_t *gcd;
    device_t *dev;
    int len;

    dev = device_get(YAMS_TYPECODE_TTY, 0); //from init_startup_fallback
    if (dev == NULL) //if error return negative value
        return -1;
    
    gcd = (gcd_t *)dev->generic_device; //from init_startup_fallback
    if (gcd == NULL)  //if error return negative value
        return -1;

    len = gcd->read(gcd, buffer, length); //from init_startup_fallback
    if (len < 0) //gcd->read function failed
        return -1;

    return len; 
}

/* returns the number of bytes written, or a negative value on error */        
int syscall_write(int fhandle, const void *buffer, int length) {
    fhandle = fhandle;
    gcd_t *gcd;
    device_t *dev;

    dev = device_get(YAMS_TYPECODE_TTY, 0); //from init_startup_fallback
    if (dev == NULL)  //if error return negative value
        return -1;

    gcd = (gcd_t *)dev->generic_device; //from init_startup_fallback
    if (gcd == NULL)  //if error return negative value
        return -1;

    gcd->write(gcd, buffer, length); //from init_startup_fallback
    return length; 
}

int syscall_exec(const char *filename){
    process_id_t pid;
    pid = process_spawn(filename);
    return pid;
}

void syscall_exit(int retval){
    process_finish(retval);
}

int syscall_join(int pid){
    int retval = process_join(pid);
    return retval;
}


/**
 * Handle system calls. Interrupts are enabled when this function is
 * called.
 *
 * @param user_context The userland context (CPU registers as they
 * where when system call instruction was called in userland)
 */
void syscall_handle(context_t *user_context)
{
    /* When a syscall is executed in userland, register a0 contains
     * the number of the syscall. Registers a1, a2 and a3 contain the
     * arguments of the syscall. The userland code expects that after
     * returning from the syscall instruction the return value of the
     * syscall is found in register v0. Before entering this function
     * the userland context has been saved to user_context and after
     * returning from this function the userland context will be
     * restored from user_context.
     */
    switch(user_context->cpu_regs[MIPS_REGISTER_A0]) {
    case SYSCALL_HALT:
        halt_kernel();
        break;
    case SYSCALL_READ:
        user_context->cpu_regs[MIPS_REGISTER_V0] =
            syscall_read(user_context->cpu_regs[MIPS_REGISTER_A1],
                         (void *)user_context->cpu_regs[MIPS_REGISTER_A2],
                         user_context->cpu_regs[MIPS_REGISTER_A3]);
        break;
    case SYSCALL_WRITE:
        user_context->cpu_regs[MIPS_REGISTER_V0] =
            syscall_write(user_context->cpu_regs[MIPS_REGISTER_A1],
                          (const void *)user_context->cpu_regs[MIPS_REGISTER_A2],
                          user_context->cpu_regs[MIPS_REGISTER_A3]);
        break;
    case SYSCALL_EXEC:
        user_context->cpu_regs[MIPS_REGISTER_V0] =
            syscall_exec((const char *)user_context->cpu_regs[MIPS_REGISTER_A1]);
        break;
    case SYSCALL_EXIT:
        syscall_exit(user_context->cpu_regs[MIPS_REGISTER_A1]);
        break;
    case SYSCALL_JOIN:
        user_context->cpu_regs[MIPS_REGISTER_V0] =
            syscall_join(user_context->cpu_regs[MIPS_REGISTER_A1]);
        break;
    default: 
        KERNEL_PANIC("Unhandled system call\n");
    }

    /* Move to next instruction after system call */
    user_context->pc += 4;
}
