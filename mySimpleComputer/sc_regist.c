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
  if (value < 0 || value > 32767)
    {
      return -1;
    }

  accumulator = value;
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
  mt_setfgcolor (1);
  printf ("%c  %c  %c  %c  %c\n", (reg_flags >> 4) & 1 ? 'E' : '_',
          (reg_flags >> 3) & 1 ? 'T' : '_', (reg_flags >> 2) & 1 ? 'P' : '_',
          (reg_flags >> 1) & 1 ? '0' : '_', (reg_flags >> 0) & 1 ? 'M' : '_');
  mt_setdefaultcolor ();
}

void
printAccumulator (void)
{
  int value;
  sc_accumulatorGet (&value);
  printf ("%04X", value);
}

void
printCounters (void)
{
}