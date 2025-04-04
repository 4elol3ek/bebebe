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
      temp = temp & 0x3FFF;
      invers (&temp);
      temp |= 0x4000;
      temp += 1;
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
  bc_box (1, 1, 15, 61, WHITE, BLACK, "Оперативная память", RED, BLACK);
  mt_gotoXY (2, 2);
  int count = 0;
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

      if ((i + 1) % 10 == 0 && i != 0)
        {
          count++;
          mt_gotoXY (2 + count, 2);
        }
      else
        {
          printf (" ");
        }
    }

  printEditCell (edit);
  bc_printeditbig (edit);
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
  int value = 0, sign = 0, temp;
  sc_memoryGet (address, &value);
  temp = value;
  if (temp >> 14)
    {
      sign = 1;
      temp &= 0x3fff;
      temp -= 1;
      invers (&temp);
    }
  else
    {
      temp &= 0x3fff;
    }
  bc_box (16, 1, 3, 61, WHITE, BLACK, "Редактируемая ячейка (формат)", WHITE,
          YELLOW);
  mt_gotoXY (17, 2);
  printf ("dec: %c%05d | oct: ", sign ? '-' : '+', temp);
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
      if (((*value) & (1 << i)) == 0)
        {
          out += (1 << i);
        }
    }
  out = out & 0x3FFF;
  *value = out;
}