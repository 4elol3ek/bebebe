#include "mySimpleComputer.h"
#define DATA_LEN 7
int
main (void)
{

  srand ((unsigned int)time (NULL)); // Инициализация rand
  sc_accumulatorInit ();
  sc_icounterInit ();
  sc_icounterSet (23);
  mt_clrscr ();

  typedef struct
  {
    int address;
    int value;
  } MemoryData;

  MemoryData data[DATA_LEN];
  data[0].address = 0;
  data[0].value = -16383;
  data[1].address = 1;
  data[1].value = 16383;
  data[2].address = 2;
  data[2].value = (rand () % (16383 - (-16383) + 1)) + (-16383);
  data[3].address = 3;
  data[3].value = (rand () % (16383 - (-16383) + 1)) + (-16383);
  data[4].address = 10;
  data[4].value = 16383;
  data[5].address = 20;
  data[5].value = -16383;
  data[6].address = 23;
  data[6].value = -1683;

  for (int i = 0; i < DATA_LEN; i++)
    {
      if (sc_memorySet (data[i].address, data[i].value) != 0)
        {
          printf ("Ошибка установки значения по адресу %d\n", data[i].address);
        }
    }

  printMem (data[6].address);
  sc_accumulatorSet (data[0].value);
  printAccumulator ();
  printFlags ();
  printCounters ();
  printCMD ();
  printInOut ();
  mt_gotoXY (30, 1);

  return 0;
}