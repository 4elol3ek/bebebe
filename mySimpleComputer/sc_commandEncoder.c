#include "mySimpleComputer.h"

int
sc_commandEncode (int sign, int command, int operand, int *value)
{
  if (value == NULL)
    {
      fprintf (stderr, "Error: the value pointer is NULL.\n");
      return -1;
    }
  if (command < 0 || command > 127)
    {
      fprintf (
          stderr,
          "Warning: The command must be in the range 0-127. Received: %d\n",
          command);
      return -1;
    }
  if (operand < 0 || operand > 127)
    {
      fprintf (
          stderr,
          "Warning: The operand must be in the range 0-127. Received: %d\n",
          operand);
      return -1;
    }
  *value = (sign & 1) << 14;
  *value |= (command & 0x7F) << 7;
  *value |= (operand & 0x7F);

  return 0;
};

int
sc_commandDecode (int val, int *sign, int *command, int *operand)
{
  if (!sign || !command || !operand)
    {
      return -1;
    }
  *sign = (val >> 14) & 1;
  *command = (val >> 7) & 0x7F;
  *operand = val & 0x7F;
  return 0;
}

void
printDecodedCommand (int value)
{
  int sign, command, operand;
  char sg = (value >> 14) ? '-' : '+';
  sc_commandDecode (value, &sign, &command, &operand);
  printf ("%c %3d %3d", sg, command, operand);
  return;
}