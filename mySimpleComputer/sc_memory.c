#include "mySimpleComputer.h"

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
  if (value < -16383 || value > 16383)
  {
    return -1;
  }
  else if (address < 0 || address >= 128)
    {
      return -2;
    }
  else if (value < 0)
  {
    int temp = value * -1;
    temp |= 0x4000;
    printf("%d", temp);
    memory[address] = temp;
  }
  else{
   memory[address] = value;
  }
  
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

void
printCell (int address, enum colors fg, enum colors bg)
{
  if (((int)fg >-1) & ((int)fg<8))
  {
    mt_setfgcolor (fg);
  }

  if (((int)bg >-1) & ((int)bg<8))
  {
    mt_setbgcolor (bg);
  }
  
  int value;
  if (sc_memoryGet (address, &value) == -1)
    {
      printf ("ERROR");
      return;
    }

  char sign = ((value >> 14) == 1) ? '-' : '+';
  value = value & 0x3FFF;
  printf ("%c%04X", sign, value);
  return;
}