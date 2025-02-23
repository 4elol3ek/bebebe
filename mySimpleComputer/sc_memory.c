#include "mySimpleComputer.h"
#include <stdio.h>

extern int memory[128];

int
sc_memoryInit (void)
{
  for (int i = 0; i < 128; i++)
    {
      memory[i] = 0;
    }
  return 0;
};

int
sc_memoryLoad (const char *filename)
{
  FILE *f = fopen (filename, "rb");
  if (!f)
    {
      return -1;
    }
  size_t read = fread (memory, sizeof (int), 128, f);
  fclose (f);
  return (read == 128) ? 0 : -1;
};

int
sc_memorySave (const char *filename)
{
  FILE *f = fopen (filename, "wb");
  if (!f)
    {
      return -1;
    }
  size_t written = fwrite (memory, sizeof (int), 128, f);
  fclose (f);
  return (written == 128) ? 0 : -1;
};

int
sc_memorySet (int address, int value)
{
  if (address < 0 || address >= 128)
    {
      return -1;
    }
  memory[address] = value;
  return 0;
};

int
sc_memoryGet (int address, int *value)
{
  if (value == NULL || address < 0 || address >= 128)
    {
      return -1;
    }
  *value = memory[address];
  return 0;
}