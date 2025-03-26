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
      sc_regSet (FLAG_OUTOFRANGE, 1);
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
      temp &= 0x7fff;
      memory[address] = temp;
      inoutAdd (address, '<', temp);
    }
  else
    {
      memory[address] = value;
      inoutAdd (address, '<', value);
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
  char sign = '+';
  int value;
  if (((int)fg > -1) & ((int)fg < 8))
    {
      mt_setfgcolor (fg);
    }

  if (((int)bg > -1) & ((int)bg < 8))
    {
      mt_setbgcolor (bg);
    }

  if (sc_memoryGet (address, &value) == -1)
    {
      printf ("ERROR");
      return;
    }
  if (value >> 14)
    {
      sign = '-';
    }
  printf ("%c%04X", sign, value);
  mt_setdefaultcolor ();
}

void
printMem (int edit)
{
  int BG = BLACK;
  int FG = WHITE;
  for (int i = 0; i < 128; i++)
    {
      if (i == edit)
        {
          printCell (i, BG, FG);
        }
      else
        {
          printCell (i, FG, BG);
        }
      printf ("  ");
      if (((i + 1) % 10 == 0) & (i != 0))
        {
          printf ("\n");
        }
    }
  printf ("\n");
  mt_gotoXY (15, 1);
  printEditCell (edit);
  return;
}

void
printBin (int value)
{
  int bits = 15;
  if (value < 0)
    {
      value *= -1;
      value |= 0x4000;
    }
  for (int i = bits - 1; i >= 0; i--)
    {
      if (i == bits - 1)
        {
          mt_setfgcolor (RED);
        }
      else if (i >= bits - 8)
        {
          mt_setfgcolor (CYAN);
        }
      else
        {
          mt_setfgcolor (MAGENTA);
        }
      printf ("%d", (value >> i) & 1);
    }

  mt_setdefaultcolor ();
}

void
printOct (int value)
{
  int res = value;
  printf ("%04o", res);
}

void
printHex (int value)
{
  int res = value;
  printf ("%04X", res);
}

void
printEditCell (int address)
{
  int value = 0, temp;
  sc_memoryGet (address, &value);
  temp = value;
  invers (&temp);
  printf ("dec: %c%05d | oct: ", (temp >> 14) ? '+' : '-', temp + 1);
  printOct (value);
  printf (" | hex: ");
  printHex (value);
  printf ("\tbin: ");
  printBin (value);
}

void
invers (int *value)
{
  int out = 0;
  for (int i = 0; i < 15; i++)
    {
      if (((*value) & (2 ^ i)) == 0)
        {
          out += 2 ^ i;
        }
    }
  out = out & 0x7FFF;
  *value = out;
}