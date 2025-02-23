#include "mySimpleComputer.h"

extern int reg_flags;

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
