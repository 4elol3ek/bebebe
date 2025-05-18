#include "myReadKey.h"
#include "mySimpleComputer.h"
#include "myTerm.h"
#include "out.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

CacheLine cache[CACHE_LINES];
extern InOutEntry inoutBuf[INOUT_SIZE];
extern inoutCount;
extern void initInterrupts (void);
extern void startExecutionMode (void);
extern void stopExecutionMode (void);
extern void nextTick (void);

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
  sc_cacheInit ();
  sc_regInit ();
  sc_memoryInit ();
  sc_accumulatorInit ();
  sc_icounterInit ();
  sc_regSet (FLAG_IGNORE, 1);

  mt_clrscr ();
  printHints ();

  // sc_accumulatorSet(14);

  // for (int i = 0; i < 128; i++)
  //   {
  //     sc_memorySet (i, i << 7 | i);
  //   }

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
            cell = (cell - 10 + (130)) % (130) - ((cell == 9) * 10)
                   - ((cell == 8) * 10);
          else if (key == KEY_DOWN)
            cell = ((cell + 10) < 128) * (cell + 10)
                   + ((cell + 10) >= 128) * (cell % 10);
          else if (key == KEY_LEFT)
            cell = (cell + 127) % 128;
          else if (key == KEY_RIGHT)
            cell = (cell + 1) % 128;
          sc_icounterSet (cell);
          break;

        case KEY_ENTER:
          stopExecutionMode ();
          sc_icounterGet (&cell);
          sc_editcurrentcell (cell, 0);
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
          char loadname[52];
          printf ("Загрузка памяти.\n");
          printf ("Введите название файла: \n");
          rk_mytermrestore ();

          tcflush (STDIN_FILENO, TCIFLUSH);

          if (fgets (loadname, sizeof loadname, stdin))
            {
              size_t len = strlen (loadname);
              if (len > 0 && loadname[len - 1] == '\n')
                loadname[len - 1] = '\0';
              sc_memoryLoad (loadname);
            }
          rk_mytermregime (0, 0, 1, 0);
          mt_clrscr ();
          printMem (0);
          printHints ();
          break;

        case KEY_S:
          stopExecutionMode ();
          mt_gotoXY (26, 1);
          char name[52];
          printf ("Сохранение памяти.\n");
          printf ("Введите название файла: \n");
          rk_mytermrestore ();

          tcflush (STDIN_FILENO, TCIFLUSH);

          if (fgets (name, sizeof name, stdin))
            {
              size_t len = strlen (name);
              if (len > 0 && name[len - 1] == '\n')
                name[len - 1] = '\0';
              sc_memorySave (name);
            }
          rk_mytermregime (0, 0, 1, 0);
          mt_clrscr ();
          printHints ();
          break;

        case KEY_I:
          stopExecutionMode ();
          memset (inoutBuf, 0, sizeof (inoutBuf));
          inoutCount = 0;
          sc_memoryInit ();
          sc_cacheInit ();
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
