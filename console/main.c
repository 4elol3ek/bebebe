#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "myTerm.h"
#include "out.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

extern InOutEntry inoutBuf[INOUT_SIZE];
extern void initInterrupts (void);
extern void startExecutionMode (void);
extern void stopExecutionMode (void);
extern void nextTick (void);

#define MEM_SIZE 128

int
printInterface (void)
{
  int cell;
  sc_icounterGet (&cell);
  printMem (cell);
  printAccumulator ();
  printFlags ();
  printCounters ();
  printCMD ();
  printInOut ();
  printCache ();
  mt_gotoXY (27, 1);
  fflush (stdout);
  return 0;
}

int
main (void)
{
  srand ((unsigned int)time (NULL));

  int cell = 0;
  int exitFlag = 0;
  keys key;

  sc_regInit ();
  sc_memoryInit ();
  sc_accumulatorInit ();
  sc_icounterInit ();
  sc_regSet (FLAG_IGNORE, 1);

  mt_clrscr ();
  printHints ();

  sc_memorySet (0, -16383);
  sc_memorySet (1, 16383);
  sc_memorySet (2, rand () % 32768 - 16383);
  sc_memorySet (3, rand () % 32768 - 16383);
  sc_memorySet (10, 16383);
  sc_memorySet (20, -16383);
  sc_memorySet (23, 13158);

  if (rk_mytermsave () != 0)
    {
      fprintf (stderr, "rk_mytermsave failed\n");
      return -1;
    }

  if (rk_mytermregime (0, 0, 1, 0) != 0)
    {
      fprintf (stderr, "rk_mytermregime failed\n");
      return -1;
    }

  initInterrupts ();

  while (!exitFlag)
    {
      printInterface ();

      if (rk_readkey (&key) != 0)
        continue;

      switch (key)
        {
        case KEY_R:
          startExecutionMode ();
          break;

        case KEY_T:
          stopExecutionMode ();
          sc_regSet (FLAG_IGNORE, 0);
          nextTick ();
          sc_regSet (FLAG_IGNORE, 1);
          break;

        case KEY_UP:
        case KEY_DOWN:
        case KEY_LEFT:
        case KEY_RIGHT:
          stopExecutionMode ();
          sc_icounterGet (&cell);
          if (key == KEY_UP)
            cell = (cell - 10 + (130)) % (130);
          else if (key == KEY_DOWN)
            cell = (cell + 10) % (130);
          else if (key == KEY_LEFT)
            cell = (cell + 127) % 128;
          else if (key == KEY_RIGHT)
            cell = (cell + 1) % 128;
          sc_icounterSet (cell);
          break;

        case KEY_ENTER:
          stopExecutionMode ();
          sc_editcurrentcell (cell);
          mt_clrscr ();
          printHints ();
          break;

        case KEY_F5:
          stopExecutionMode ();
          sc_editaccumulator ();
          break;

        case KEY_F6:
          stopExecutionMode ();
          sc_editicounter (&cell);
          break;

        case KEY_L:
          stopExecutionMode ();
          mt_gotoXY (26, 1);
          printf ("Сохранение памяти...\n");
          fflush (stdout);
          break;

        case KEY_S:
          stopExecutionMode ();
          mt_gotoXY (26, 1);
          printf ("Загрузка памяти...\n");
          fflush (stdout);
          break;

        case KEY_I:
          stopExecutionMode ();
          memset (inoutBuf, 0, sizeof (inoutBuf));
          sc_memoryInit ();
          sc_accumulatorInit ();
          sc_regInit ();
          sc_regSet (FLAG_IGNORE, 1);
          mt_clrscr ();
          printHints ();
          break;

        case KEY_ESC:
          stopExecutionMode ();
          exitFlag = 1;
          break;

        default:
          break;
        }
    }

  rk_mytermrestore ();
  mt_setdefaultcolor ();
  mt_clrscr ();
  printf ("Выход из программы.\n");
  return 0;
}
