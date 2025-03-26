#include "mySimpleComputer.h"

extern int reg_flags;
extern int accumulator;
extern int instruction_counter;

int
sc_regInit (void)
{
  reg_flags = 0;
  return 0;
};

int
sc_regGet (int flag, int *value)
{
  if (!value)
    {
      return -1;
    }
  *value = (reg_flags & flag) ? 1 : 0;
  return 0;
};

int
sc_regSet (int flag, int value)
{
  if (value != 0 && value != 1)
    {
      return -1;
    }
  if (value == 1)
    {
      reg_flags |= flag;
    }
  else
    {
      reg_flags &= ~flag;
    }
  return 0;
}

int
sc_accumulatorInit (void)
{
  accumulator = 0;
  return 0;
}

int
sc_accumulatorSet (int value)
{
  if (value < 0)
    {
      int temp = value * -1;
      temp |= 0x4000;
      accumulator = temp;
      return 0;
    }
  else if (value > 32736)
    {
      return -1;
    }
  else
    {
      accumulator = value;
    }
  return 0;
}

int
sc_accumulatorGet (int *value)
{
  if (value == NULL)
    {
      return -1;
    }
  *value = accumulator;
  return 0;
}

int
sc_icounterInit (void)
{
  instruction_counter = 0;
  return 0;
}

int
sc_icounterSet (int value)
{
  if (value < 0 || value > 128)
    {
      return -1;
    }

  instruction_counter = value;
  return 0;
}

int
sc_icounterGet (int *value)
{
  if (value == NULL)
    {
      return -1;
    }
  *value = instruction_counter;
  return 0;
}

void
printFlags (void)
{
  mt_gotoXY (1, 100);
  mt_setfgcolor (1);
  printf ("%c  %c  %c  %c  %c\n", (reg_flags >> 4) & 1 ? 'P' : '_',
          (reg_flags >> 3) & 1 ? '0' : '_', (reg_flags >> 2) & 1 ? 'M' : '_',
          (reg_flags >> 1) & 1 ? 'T' : '_', reg_flags & 1 ? 'E' : '_');
  mt_setdefaultcolor ();
}

void
printAccumulator (void)
{
  mt_gotoXY (1, 72);
  int value, temp, sign = '+', cmd, oper;
  sc_accumulatorGet (&value);
  temp = value;
  sc_commandDecode (value, &sign, &cmd, &oper);
  if (value >> 14)
    {
      sign = '-';
      printf ("sc: %c%02X%02X hex: %c%04X", sign, cmd, oper, sign, temp);
    }
  else
    {
      sign = '+';
      printf ("sc: %c%02X%02X hex: %c%04X", sign, cmd, oper, sign, temp);
    }
}

void
printCounters (void)
{
  mt_gotoXY (3, 72);
  int value, temp, dec = 0;
  char sign;
  sc_icounterGet (&value);
  if (value >> 14)
    {
      temp = value & 0x3fff;
      sign = '-';
    }
  else
    {
      temp = value;
      sign = '+';
    }
  printf ("T: %02d\tIC: %c%04X", dec, sign, temp);
}