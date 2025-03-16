#include "mySimpleComputer.h"

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

  sc_icounterSet (10);
  printf ("sc_icounterSet status : 0\n");

  sc_memoryGet (0, &memValue);
  sc_commandDecode (memValue, &sign, &command, &operand);
  printf ("Decoded value of cell 0: sign=%d, command=%d, operand=%d\n", sign,
          command, operand);

  return 0;
}