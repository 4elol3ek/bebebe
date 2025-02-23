#include "mySimpleComputer.h" // Header file for the mySimpleComputer library
#include <stdio.h>

int
main (void)
{
  int value;
  int encoded;
  int sign, command, operand;
  int address, memValue, comm, oper;

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

  if (sc_commandEncode (0, comm, oper, &encoded) != 0)
    {
      printf ("Error: Failed to encode command.\n");
      return 1;
    }
  printf ("Encoded command: %d\n", encoded);

  if (sc_commandDecode (encoded, &sign, &command, &operand) != 0)
    {
      printf ("Error: Failed to decode command.\n");
      return 1;
    }
  printf ("Decoded command: sign = %d, command = %d, operand = %d\n", sign,
          command, operand);

  return 0;
}
