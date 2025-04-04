#include "mySimpleComputer.h"

static InOutEntry inoutBuf[INOUT_SIZE];
static int inoutCount = 0;

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
sc_commandDecode (int value, int *sign, int *command, int *operand)
{
  if (!sign || !command || !operand)
    {
      return -1;
    }
  *sign = value & 0x4000;
  *command = value >> 7 & 0x7f;
  *operand = value & 0x7F;
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

void
printCMD (void)
{
  int ic;
  bc_box (4, 84, 3, 23, WHITE, BLACK, "Команда", RED, BLACK);
  mt_gotoXY (5, 90);
  if (sc_icounterGet (&ic) != 0)
    {
      printf ("! +FF : FF");
      return;
    }

  if (ic < 0 || ic >= 100)
    {
      printf ("! +FF : FF");
      return;
    }

  int value;
  if (sc_memoryGet (ic, &value) != 0)
    {
      printf ("! +FF : FF");
      return;
    }
  int sign, cmd, operand;
  if (sc_commandDecode (value, &sign, &cmd, &operand) != 0)
    {
      printf ("! +FF : FF");
      return;
    }
  char s = (sign == 0) ? '+' : '-';
  printf ("%c %02X : %02X", s, cmd, operand);
}

void
inoutAdd (int address, char type, int value)
{
  if (inoutCount == INOUT_SIZE)
    {
      for (int i = 0; i < INOUT_SIZE - 1; i++)
        {
          inoutBuf[i] = inoutBuf[i + 1];
        }
      inoutCount = INOUT_SIZE - 1;
    }
  inoutBuf[inoutCount].address = address;
  inoutBuf[inoutCount].type = type;
  inoutBuf[inoutCount].value = value;
  inoutCount++;
}

void
printInOut (void)
{
  bc_box (19, 68, 7, 12, WHITE, BLACK, "IN--OUT", RED, BLACK);
  for (int i = 0; i < inoutCount; i++)
    {
      int address = inoutBuf[i].address;
      char type = inoutBuf[i].type;
      int value = inoutBuf[i].value;

      mt_gotoXY (20 + i, 69);
      printf ("%03d%c %c", address, type, (value >> 14) ? '-' : '+');
      printHex (value);
    }
}