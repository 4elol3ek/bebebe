#include "mySimpleComputer.h"
#include <stdio.h>

const int valid_cmds[] = {
  /* IN-OUT */
  0x00, /* NOP     */
  0x01, /* CPUINFO */
  0x0A, /* READ    */
  0x0B, /* WRITE   */
  /* L/S Accumulator */
  0x14, /* LOAD    */
  0x15, /* STORE   */
  /* Аrithmetic functions */
  0x1E, /* ADD     */
  0x1F, /* SUB     */
  0x20, /* DIVIDE  */
  0x21, /* MUL     */
  /* Transfer control */
  0x28, /* JUMP    */
  0x29, /* JNEG    */
  0x2A, /* JZ      */
  0x2B, /* HALT    */
  /* User func */
  0x33, /* NOT     */
  0x34, /* AND     */
  0x35, /* OR      */
  0x36, /* XOR     */
  0x37, /* JNS     */
  0x38, /* JC      */
  0x39, /* JNC     */
  0x3A, /* JP      */
  0x3B, /* JNP     */
  0x3C, /* CHL     */
  0x3D, /* SHR     */
  0x3E, /* RCL     */
  0x3F, /* RCR     */
  0x40, /* NEG     */
  0x41, /* ADDC    */
  0x42, /* SUBC    */
  0x43, /* LOGLC   */
  0x44, /* LOGRC   */
  0x45, /* RCCL    */
  0x46, /* RCCR    */
  0x47, /* MOVA    */
  0x48, /* MOVR    */
  0x49, /* MOVCA   */
  0x4A, /* MOVCR   */
  0x4B, /* ADDC    */
  0x4C  /* SUBC    */
};
static const size_t valid_cnt = sizeof (valid_cmds) / sizeof (*valid_cmds);

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

int
isvalidcommands ()
{
  for (int i = 0; i < 128; i++)
    {
      int value, sign, command, operand;
      if (sc_memoryGet (i, &value, 0) != 0)
        continue;
      if (sc_commandDecode (value, &sign, &command, &operand) != 0)
        continue;

      if (sign == 0)
        {
          int is_cmd = 0;
          for (size_t x = 0; x < valid_cnt; ++x)
            {
              if (valid_cmds[x] == command)
                {
                  is_cmd = 1;
                  break;
                }
            }
          if (!is_cmd)
            {
              sc_regSet (FLAG_INVALIDCMD, 1);
              return 1;
            }
        }
    }
  return 0;
}
