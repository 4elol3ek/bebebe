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