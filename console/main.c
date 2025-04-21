#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "myTerm.h"
#include "out.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
extern InOutEntry inoutBuf[INOUT_SIZE];

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
      printf ("Ошибка сохранения настроек терминала.\n");
      return -1;
    }
  if (rk_mytermregime (0, 0, 1, 0) != 0)
    {
      printf ("Ошибка установки неканонического режима.\n");
      return -1;
    }

  while (!exitFlag)
    {
      printInterface ();

      if (rk_readkey (&key) != 0)
        continue;

      switch (key)
        {
        case KEY_UP:
          if (cell == 8 || cell == 9)
            cell += 110;
          else
            cell = (cell - 10 + (MEM_SIZE + 2)) % (MEM_SIZE + 2);
          sc_icounterSet (cell);
          break;
        case KEY_DOWN:
          if (cell == 118 || cell == 119)
            cell -= 110;
          else
            cell = (cell + 10) % (MEM_SIZE + 2);
          sc_icounterSet (cell);
          break;
        case KEY_LEFT:
          cell = (cell - 1 + MEM_SIZE) % MEM_SIZE;
          sc_icounterSet (cell);
          break;
        case KEY_RIGHT:
          cell = (cell + 1) % MEM_SIZE;
          sc_icounterSet (cell);
          break;
        case KEY_ENTER:
          sc_editcurrentcell (cell);
          mt_clrscr ();
          printHints ();
          break;
        case KEY_F5:
          sc_editaccumulator ();
          break;
        case KEY_F6:
          sc_editicounter (&cell);
          break;
        case KEY_L:
          mt_gotoXY (26, 1);
          rk_mytermsave ();
          printf ("Сохранение памяти...\n");
          fflush (stdout);
          break;
        case KEY_S:
          mt_gotoXY (26, 1);
          rk_mytermrestore ();
          printf ("Загрузка памяти...\n");
          fflush (stdout);
          break;
        case KEY_I:
          memset (inoutBuf, 0, sizeof (InOutEntry) * 5);
          sc_memoryInit ();
          sc_accumulatorInit ();
          sc_regInit ();
          sc_regSet (FLAG_IGNORE, 1);
          mt_clrscr ();
          printHints ();
          break;
        case KEY_ESC:
          exitFlag = 1;
          break;
        default:
          break;
        }
    }

  rk_mytermrestore ();
  printf ("Выход из программы.\n");

  return 0;
}
