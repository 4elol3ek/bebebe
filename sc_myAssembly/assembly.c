#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_SIZE 128

int
commandToCode (const char *cmd)
{
  if (strcmp (cmd, "READ") == 0)
    return 0x0A;
  if (strcmp (cmd, "WRITE") == 0)
    return 0x0B;
  if (strcmp (cmd, "LOAD") == 0)
    return 0x14;
  if (strcmp (cmd, "STORE") == 0)
    return 0x15;
  if (strcmp (cmd, "ADD") == 0)
    return 0x1E;
  if (strcmp (cmd, "SUB") == 0)
    return 0x1F;
  if (strcmp (cmd, "DIVIDE") == 0)
    return 0x20;
  if (strcmp (cmd, "MUL") == 0)
    return 0x21;
  if (strcmp (cmd, "JUMP") == 0)
    return 0x28;
  if (strcmp (cmd, "JNEG") == 0)
    return 0x29;
  if (strcmp (cmd, "JZ") == 0)
    return 0x2A;
  if (strcmp (cmd, "HALT") == 0)
    return 0x2B;
  if (strcmp (cmd, "CPUINFO") == 0)
    return 0x01;
  if (strcmp (cmd, "NOP") == 0)
    return 0x00;
  return -1;
}

int
parseLine (const char *line, int *address, int *value)
{
  char cmd[16];
  int operand;

  if (sscanf (line, "%d = +%d", address, value) == 2)
    {
      return 1; // Direct value assignment
    }
  if (sscanf (line, "%d %15s %d", address, cmd, &operand) == 3)
    {
      int code = commandToCode (cmd);
      if (code == -1)
        return -1;
      *value = code << 7 | operand;
      return 0;
    }
  return -1; // Invalid line
}

int
main (int argc, char *argv[])
{
  if (argc != 3)
    {
      fprintf (stderr, "Usage: %s input.sa output.o\n", argv[0]);
      return 1;
    }

  FILE *fin = fopen (argv[1], "r");
  if (!fin)
    {
      perror ("Error opening input file");
      return 1;
    }

  FILE *fout = fopen (argv[2], "wb");
  if (!fout)
    {
      perror ("Error opening output file");
      fclose (fin);
      return 1;
    }

  int memory[MEM_SIZE] = { 0 };
  char line[256];

  while (fgets (line, sizeof (line), fin))
    {
      // remove comment
      char *comment = strchr (line, ';');
      if (comment)
        *comment = '\0';

      int address, value;
      int type = parseLine (line, &address, &value);
      if (type == -1 || address < 0 || address >= MEM_SIZE)
        {
          fprintf (stderr, "Syntax error: %s\n", line);
          fclose (fin);
          fclose (fout);
          return 1;
        }
      memory[address] = value;
    }

  fwrite (memory, sizeof (int), MEM_SIZE, fout);
  fclose (fin);
  fclose (fout);
  return 0;
}
