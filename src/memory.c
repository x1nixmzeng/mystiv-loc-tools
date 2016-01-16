#include "memory.h"

#include <stdio.h>
#include <stdlib.h>

#define DBG_MEMORY 0

#if DBG_MEMORY
int g_mem_alloc = 0;
int g_mem_freed = 0;

#define NUM_ADDRS 3000
int addrs[NUM_ADDRS] = { 0 };
int addls[NUM_ADDRS] = { 0 };
#endif

void* mem_alloc(int length)
{
  void* addr;

  addr = (void* )malloc(length);

#if DBG_MEMORY
  printf("%08x %u\n", addr, length);
  ++g_mem_alloc;

  {
    int i;

    i = 0;
    while (i < NUM_ADDRS) {
      if (addrs[i] == 0) {
        addrs[i] = addr;
        addls[i] = length;
        break;
      }
      ++i;
    }
  }

#endif

  return addr;
}

void* mem_realloc(void* addr, int new_length)
{
  return realloc(addr, new_length);
}

void mem_free(void* addr)
{
#if DBG_MEMORY
  if (addr == 0) {
    printf("Memory address is already freed\n");
  }

  {
    int i;

    i = 0;
    while (i < NUM_ADDRS) {
      if (addrs[i] == addr) {
        addrs[i] = 0;
        addls[i] = 0;
        break;
      }
      ++i;
    }
  }

  printf("%08x\n", addr);
  ++g_mem_freed;
#endif

  free(addr);
}

void mem_dbg()
{
#if DBG_MEMORY
  printf(" mem_alloc: %u\n", g_mem_alloc);
  printf("  mem_free: %u\n", g_mem_freed);

  {
    int i;

    i = 0;
    while (i < NUM_ADDRS) {
      if (addrs[i] != 0) {
        printf("Leak at %08x (%u bytes)\n", addrs[i], addls[i]);
      }
      ++i;
    }
  }

  if (g_mem_alloc != g_mem_freed) {
    if (g_mem_alloc > g_mem_freed) {
      printf("Memory leaks detected\n");
    } else {
      printf("Mismatched calls to mem_free\n");
    }
  }
#endif
}
