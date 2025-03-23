#include "../include/myTerm.h"
#include <stdio.h>

int
main ()
{
  printf ("Тест библиотеки myTerm\n");

  mt_clrscr ();
  mt_gotoXY (5, 10);
  mt_setfgcolor (RED);
  printf ("Hello, World!\n");
  mt_setdefaultcolor ();

  return 0;
}
