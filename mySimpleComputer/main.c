#include "mySimpleComputer.h"
#include <stdio.h>

int
main (void)
{
  sc_memoryInit ();
  sc_regInit ();

  sc_memorySet (10, 9999);

  int val = 0;
  if (sc_memoryGet (10, &val) == 0)
    {
      printf ("Memory[10] = %d\n", val);
    }

  int encoded = 0;
  if (sc_commandEncode (0, 11, 80, &encoded) == 0)
    {
      printf ("Encoded = %d\n", encoded);
      int sign, cmd, op;
      sc_commandDecode (encoded, &sign, &cmd, &op);
      printf ("Decoded => sign=%d, cmd=%d, op=%d\n", sign, cmd, op);
    }

  return 0;
}