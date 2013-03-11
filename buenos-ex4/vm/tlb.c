/*
 * TLB handling
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
 * $Id: tlb.c,v 1.6 2004/04/16 10:54:29 ttakanen Exp $
 *
 */

#include "kernel/panic.h"
#include "kernel/assert.h"
#include "vm/tlb.h"
#include "vm/pagetable.h"

void tlb_modified_exception(void)
{
  thread_table_t* thread = thread_get_current_thread_entry();

  if(thread->context->status & 0x8)
    {
      kprintf("Access violation from TLB load exception: %d",
	      thread_get_current_thread());
    }
  
  process_finish(-1); //kill thread
  thread_switch(); //switching to another active thread
  
  else
    {
      KERNEL_PANIC("Access violation from TLB store exception");
    }
}

/* exception_handler handles the exceptions from load and store
 * since it should be handled the same way - only the error messages
 * is different
 */ 

void tlb_exception_handler(int store)
{
  thread_table_t* thread = thread_get_current_thread_entry(); //type from thread.h
  page_table_t* pagetable = thread->pagetable;
  tlb_entry_t* entry = pagetable->entries;
  int i;

  /* creating state variable filled through tlb_get_exception
   * as mentioned in the assignment 
   */
  tlb_exception_state_t exception_state*;
  _tlb_get_exception_state(exception_state);
  
  /* running through table comparing the upper 19 bits of
   * the virtual address. Also check if the dirty bit fits
   * page type 
   */
  for(i = 0; i < (int)pagetable->valid_count; i++)
    {
      if(exception_state->badvaddr >> 13 == entry[i].vpn2 &&
	 ((ADDR_IS_ON_EVEN_PAGE(exception_state->badvaddr) && entry[i].V0) ||
	   (ADDR_IS_ON_ODD_PAGE(exception_state->badvaddr) && entry[i].V1)))
	{
	  _tlb_write_random(&entry[i]);
	  return;
	}
    }

  /* access violation - user process mode */
  if(thread->context->status & 0x8)
    {
      if(store) //exception for store
	{
	  kprintf("Access violation from TLB store exception: %d", 
		  thread_get_current_thread());
	}
      else //exception for load
	{
	  kprintf("Access violation from TLB load exception: %d",
		  thread_get_current_thread());
	}
      process_finish(-1); //kill thread
      thread_switch(); //switching to another active thread
    }
  else
    {
      if(store) //exception for store
	{
	  KERNEL_PANIC("Access violation from TLB store exception");
	}
      else //exception for load
	{
	  KERNEL_PANIC("Access violation from TLB load exception");
	}
    }
}

void tlb_load_exception(void)
{
  tlb_exception_handler(1); 
}

void tlb_store_exception(void)
{
  tlb_exception_handler(0);
}
