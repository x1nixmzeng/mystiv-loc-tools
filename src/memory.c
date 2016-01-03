#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

//#define DBG_MEMORY

#ifdef DBG_MEMORY
int g_mem_alloc = 0;
int g_mem_freed = 0;

int dbg[3000] = { 0 };
#endif

void* mem_alloc(int length)
{
  void* addr;

  addr = (void* )malloc(length);

#ifdef DBG_MEMORY
  printf("%08x %u\n", addr, length);

  dbg[g_mem_alloc] = (int)addr;
  ++g_mem_alloc;
#endif

  return addr;
}

void mem_free(void* addr)
{
#ifdef DBG_MEMORY
  int i, done;

  ++g_mem_freed;

  done = 0;

  for( i = 0; i < 3000; ++i ) {
    if( dbg[i] == (int)addr ) {
      //dbg[i] = 0;
      done = 1;
      break;
    }
  }

  if( done != 1 ) {
    printf("Failed to free address %08x\n", addr);
  }
#endif

  free(addr);
}

void mem_dbg()
{
#ifdef DBG_MEMORY
  int leaked, i;

  printf("Alloc: %u\n", g_mem_alloc);
  printf("Freed: %u\n", g_mem_freed);

  leaked = g_mem_freed - g_mem_alloc;

  printf("LEAKED: %u\n", leaked);

  leaked = 0;
  for( i = 0; i < 3000; ++i ) {
    if( dbg[i] != 0 ) {
      ++leaked;
    }
  }

  printf("Known leaks: %u\n", leaked);
#endif
}
