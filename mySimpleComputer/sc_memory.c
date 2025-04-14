#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "myTerm.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

extern int memory[128];

int
sc_memoryInit (void)
{
  for (int i = 0; i < 128; i++)
    {
      memory[i] = 0;
    }
  return 0;
};

int
sc_memoryLoad (const char *filename)
{
  FILE *f = fopen (filename, "rb");
  if (!f)
    {
      return -1;
    }
  size_t read = fread (memory, sizeof (int), 128, f);
  fclose (f);
  return (read == 128) ? 0 : -1;
};

int
sc_memorySave (const char *filename)
{
  FILE *f = fopen (filename, "wb");
  if (!f)
    {
      return -1;
    }
  size_t written = fwrite (memory, sizeof (int), 128, f);
  fclose (f);
  return (written == 128) ? 0 : -1;
};

int
sc_memorySet (int address, int value)
{
  if (value < -16383 || value > 16383)
    {
      sc_regSet (FLAG_OUTOFRANGE, 1);
      return -1;
    }
  else if (address < 0 || address >= 128)
    {
      return -2;
    }
  else if (value < 0)
    {
      int temp = value * -1;
      temp = temp & 0x3FFF;
      invers (&temp);
      temp |= 0x4000;
      temp += 1;
      memory[address] = temp;
      inoutAdd (address, '>', temp);
    }
  else
    {
      memory[address] = value;
      inoutAdd (address, '>', value);
    }
  return 0;
};

int
sc_memoryGet (int address, int *value, int inout)
{
  if (value == NULL || address < 0 || address >= 128)
    {
      return -1;
    }
  *value = memory[address];
  if (inout) inoutAdd (address, '<', *value);
  return 0;
}

void
sc_editcurrentcell (int address)
{
  int val;
  int row = (address / 10) + 2;
  int col = (address % 10) * 6 + 2;

  struct termios oldt, newt;
  tcgetattr (STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag |= (ICANON | ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &newt);

  mt_gotoXY (row, col);
  mt_setbgcolor (WHITE);
  mt_setfgcolor (BLACK);
  printf ("     ");
  mt_gotoXY (row, col);
  fflush (stdout);

  char buf[16];
  if (fgets (buf, sizeof (buf), stdin) != NULL)
    {
      val = atoi (buf);
      sc_memorySet (address, val);

      mt_setbgcolor (BLACK);
      mt_setfgcolor (WHITE);
      mt_gotoXY (row, col);
      printf ("+%04X", (unsigned short)(val & 0x3FFF));
    }

  tcsetattr (STDIN_FILENO, TCSANOW, &oldt);
  mt_setdefaultcolor ();
  fflush (stdout);
}

void
sc_editaccumulator (void)
{
  int val;

  struct termios oldt, newt;
  tcgetattr (STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag |= (ICANON | ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &newt);

  mt_gotoXY (5, 77);
  mt_setbgcolor (WHITE);
  mt_setfgcolor (BLACK);
  printf ("     ");
  mt_gotoXY (5, 77);
  fflush (stdout);

  char buf[16];
  if (fgets (buf, sizeof (buf), stdin) != NULL)
    {
      val = atoi (buf);
      sc_accumulatorSet(val);
      mt_setbgcolor (BLACK);
      mt_setfgcolor (WHITE);
      mt_gotoXY (5, 77);
      printf ("+%04X", (unsigned short)(val & 0x3FFF));
    }

  tcsetattr (STDIN_FILENO, TCSANOW, &oldt);
  mt_setdefaultcolor ();
  fflush (stdout);
}

void
sc_editicounter (void)
{
  int val;

  struct termios oldt, newt;
  tcgetattr (STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag |= (ICANON | ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &newt);

  mt_gotoXY (5, 77);
  mt_setbgcolor (WHITE);
  mt_setfgcolor (BLACK);
  printf ("     ");
  mt_gotoXY (5, 77);
  fflush (stdout);

  char buf[16];
  if (fgets (buf, sizeof (buf), stdin) != NULL)
    {
      val = atoi (buf);
      sc_icounterSet(val);
      mt_setbgcolor (BLACK);
      mt_setfgcolor (WHITE);
      mt_gotoXY (5, 77);
      printf ("+%04X", (unsigned short)(val & 0x3FFF));
    }

  tcsetattr (STDIN_FILENO, TCSANOW, &oldt);
  mt_setdefaultcolor ();
  fflush (stdout);
}