#include "mySimpleComputer.h"

int
printMem ()
{
  for (int i = 0; i < 128; i++)
    {
      printCell (i, 6, 0);
      printf ("  ");
      if (((i + 1) % 16 == 0) & (i != 0))
        {
          printf ("\n");
        }
    }
  printf ("\n");
  return 0;
}

int
main (void)
{
  int value;
  int sign, command, operand, acc_value;
  int address, memValue, comm, oper;

  sc_accumulatorInit ();
  if (sc_memoryInit () != 0)
    {
      printf ("Error: Failed to initialize memory.\n");
      return 1;
    }

  printf ("Enter memory cell address (from 0 to 127): ");
  if (scanf ("%d", &address) != 1 || address < 0 || address >= 128)
    {
      printf ("Error: Invalid address.\n");
      return 1;
    }

  printf ("Enter value for the memory cell: ");
  if (scanf ("%d", &memValue) != 1)
    {
      printf ("Error: Invalid value.\n");
      return 1;
    }

  if (sc_memorySet (address, memValue) != 0)
    {
      printf ("Error: Failed to set value in memory.\n");
      return 1;
    }

  if (sc_memoryGet (address, &value) != 0)
    {
      printf ("Error: Failed to retrieve value from memory.\n");
      return 1;
    }
  printf ("Memory cell %d value: %d\n", address, value);

  printMem ();
  mt_setdefaultcolor ();
  sc_regSet (FLAG_OUTOFRANGE, 1);
  sc_regSet (FLAG_DIVZERO, 1);
  sc_regSet (FLAG_OVERFLOW, 1);
  sc_regSet (FLAG_IGNORE, 1);
  sc_regSet (FLAG_INVALIDCMD, 1);

  printFlags ();

  printf ("Enter command code for encoding: ");
  if (scanf ("%d", &comm) != 1)
    {
      printf ("Error: Invalid command code.\n");
      return 1;
    }
  printf ("Enter command operand: ");
  if (scanf ("%d", &oper) != 1)
    {
      printf ("Error: Invalid operand.\n");
      return 1;
    }

  if (sc_commandEncode (0, comm, oper, &acc_value) != 0)
    {
      printf ("Error: Failed to encode command.\n");
      return 1;
    }
  printf ("Encoded command: %d\n", acc_value);
  sc_accumulatorSet (acc_value);
  sc_accumulatorGet (&acc_value);
  printf ("Accumulate command: %d\n", acc_value);

  sc_accumulatorGet (&acc_value);
  if (sc_commandDecode (acc_value, &sign, &command, &operand) != 0)
    {
      printf ("Error: Failed to decode command.\n");
      return 1;
    }
  printf ("Decoded command: sign = %d, command = %d, operand = %d\n", sign,
          command, operand);

  sc_accumulatorGet (&acc_value);
  printf ("sc_accumulatorSet status : %d\n", acc_value);

  printf ("sc_icounterSet status : %d\n", sc_icounterSet (10));

  sc_memoryGet (0, &memValue);
  sc_commandDecode (memValue, &sign, &command, &operand);
  printf ("Decoded value of cell 0: sign=%d, command=%d, operand=%d\n", sign,
          command, operand);

  return 0;
}