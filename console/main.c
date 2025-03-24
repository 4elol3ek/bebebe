#include "mySimpleComputer.h"

int
main (void)
{
  mt_clrscr ();

  typedef struct
  {
    int address;
    int value;
  } MemoryData;

  int value, editcell = 32;

  MemoryData data[6];
  data[0].address = 0;
  data[0].value = -16383;
  data[1].address = 1;
  data[1].value = 16383;

  srand ((unsigned int)time (NULL)); // Инициализация rand

  // Случайное число 1 в диапазоне [-16384, +16383]
  data[2].address = 2;
  data[2].value = (rand () % (16383 - (-16383) + 1)) + (-16383);

  // Случайное число 2 в диапазоне [-16383, +16383]
  data[3].address = 3;
  data[3].value = (rand () % (16383 - (-16383) + 1)) + (-16383);

  data[4].address = 16;
  data[4].value = 16383;
  data[5].address = 32;
  data[5].value = -16383;

  for (int i = 0; i < 6; i++)
    {
      if (sc_memorySet (data[i].address, data[i].value) != 0)
        {
          printf ("Ошибка установки значения по адресу %d\n", data[i].address);
        }
    }

  printMem (editcell);
  for (int i = 0; i < 6; i++)
    {
      if (sc_memoryGet (data[i].address, &value) != 0)
        {
          printf ("Ошибка установки значения по адресу %d\n", data[i].address);
        }
      if (value >> 14)
        {
          int temp = value & 0x3FFF;
          printf ("Memory cell %d value: -%d", data[i].address, temp);
        }
      else
        {
          printf ("Memory cell %d value:  %d", data[i].address, value);
        }

      printf ("\nHex: ");
      printHex (value, 1);
      printf ("\nOct: ");
      printOct (value, 1);
      printf ("\nBin: ");
      printBin (value);
      printf ("\n\n");
    }

  printf ("Номер ячейки выделенной для примера редактирования: ");
  mt_setfgcolor (GREEN);
  printf ("%d\n\n", editcell);
  mt_setdefaultcolor ();
  return 0;
}