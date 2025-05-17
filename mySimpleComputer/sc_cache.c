#include "mySimpleComputer.h"
#include "myTerm.h"
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

extern int tick_delay;
extern int memory[128];

extern CacheLine cache[CACHE_LINES];

void
sc_cacheInit (void)
{
  // mt_gotoXY(27,1);
  // printf("%p in init\n", (void*)cache);
  // fflush(stdout);
  memset (cache, 0, sizeof (cache));
}

void
updateAges (int idx)
{
  for (int i = 0; i < CACHE_LINES; ++i)
    if (cache[i].valid)
      cache[i].age++;
  cache[idx].age = 0;
}

int
selectLine (void)
{
  for (int i = 0; i < CACHE_LINES; ++i)
    if (!cache[i].valid)
      return i;
  int idx = 0;
  unsigned int max_age = cache[0].age;
  for (int i = 1; i < CACHE_LINES; ++i)
    {
      if (cache[i].age > max_age)
        {
          max_age = cache[i].age;
          idx = i;
        }
    }
  return idx;
}

int
lineSize (int tag)
{
  return (tag == 12 ? 8 : 10);
}

int
sc_cacheRead (int address, int *value)
{
  if (address < 0 || address >= MEM_SIZE)
    {
      sc_regSet (FLAG_OUTOFRANGE, 1);
      return -1;
    }
  int tag = address / LINE_SIZE;
  int offset = address % LINE_SIZE;

  for (int i = 0; i < CACHE_LINES; ++i)
    {
      if (cache[i].valid && cache[i].tag == tag)
        {
          updateAges (i);
          *value = cache[i].data[offset];
          tick_delay = 2;
          return 0; // cache-hit
        }
    }
  /* cache-miss */
  int idx = selectLine ();
  sc_cacheFlush (idx);
  int size = lineSize (tag);
  int base = tag * LINE_SIZE;
  for (int j = 0; j < size; ++j)
    cache[idx].data[j] = memory[base + j];
  cache[idx].valid = 1;
  cache[idx].dirty = 0;
  cache[idx].tag = tag;
  updateAges (idx);
  *value = cache[idx].data[offset];
  tick_delay = 10;
  return 0;
}

int
sc_cacheWrite (int address, int value)
{
  if (address < 0 || address >= MEM_SIZE)
    {
      sc_regSet (FLAG_OUTOFRANGE, 1);
      return -1;
    }
  int tag = address / LINE_SIZE;
  int offset = address % LINE_SIZE;
  for (int i = 0; i < CACHE_LINES; ++i)
    {
      if (cache[i].valid && cache[i].tag == tag)
        {
          cache[i].data[offset] = value;
          cache[i].dirty = 1;
          updateAges (i);
          tick_delay = 2;
          return 0;
        }
    }
  /* промах: загрузка */
  int idx = selectLine ();
  sc_cacheFlush (idx);
  int size = lineSize (tag);
  int base = tag * LINE_SIZE;
  for (int j = 0; j < size; ++j)
    cache[idx].data[j] = memory[base + j];
  cache[idx].valid = 1;
  cache[idx].tag = tag;
  cache[idx].data[offset] = value;
  cache[idx].dirty = 1;
  updateAges (idx);
  tick_delay = 10;
  return 0;
}

int
sc_cacheFlush (int line)
{
  ;
  int it = (line == -1) ? CACHE_LINES : 1;
  line = (line == -1) ? 0 : line;
  for (int i = 0; i < it; ++i)
    {
      if (cache[i + line].valid && cache[i + line].dirty)
        {
          int tag = cache[i + line].tag;
          int size = lineSize (tag);
          int base = tag * LINE_SIZE;
          for (int j = 0; j < size; ++j)
            {
              memory[base + j] = cache[i + line].data[j];
            }
          cache[i + line].dirty = 0;
        }
    }
}