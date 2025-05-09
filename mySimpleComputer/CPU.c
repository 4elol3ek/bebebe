#include "mySimpleComputer.h"
#include "myTerm.h"
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>


int timeout = 500000;
static int temp;

extern int memory[128];
extern int printInterface (void);
static int (*commands[128]) (int operand) = { NULL };
static int initialized = 0;


// IN_OUT FUNC START

static inline int
op_NOP (int operand)
{
  (void)operand;
  return 0;
}

static inline int
op_CPUINFO (int operand)
{
  (void)operand;
  mt_gotoXY (27, 1);
  printf ("Тилепов Данияр Илимбекович, ИС-342");
  return 0;
}

static inline int
op_READ (int operand)
{
  sc_editcurrentcell (operand);
  return 0;
}

static inline int
op_WRITE (int operand)
{
  int a;
  mt_gotoXY (27, 1);
  printf ("                                               \n"
          "                         ");
  mt_gotoXY (27, 1);
  sc_memoryGet (operand, &a, 0);
  printf ("Ячейка[%03d] hex: %04X, dec:%d", operand, a, a);
  return sc_icounterSet (operand);
}

static inline int
op_LOAD (int operand)
{
  int value = 0;
  if (sc_memoryGet (operand, &value, 1) != 0)
    return -1;
  return sc_accumulatorSet (value);
}

static inline int
op_STORE (int operand)
{
  int value = 0;
  if (sc_accumulatorGet (&value) != 0)
    return -1;
  return sc_memorySet (operand, value);
}

// IN_OUT FUNC END

// ALU FUNC START

static inline int
com_ADD (int operand)
{
  int a, b, res;
  sc_accumulatorGet (&a);
  sc_memoryGet (operand, &b, 1);

  res = a + b;
  if (res < -0x7FFF || res > 0x7FFF)
    {
      sc_regSet (FLAG_OVERFLOW, 1);
      res &= 0x7fff;
    }
  return sc_accumulatorSet (res);
}

static inline int
com_SUB (int operand)
{
  int a, b, res;
  sc_accumulatorGet (&a);
  sc_memoryGet (operand, &b, 1);

  res = a - b;
  if (res < -0x7FFF || res > 0x7FFF)
    {
      sc_regSet (FLAG_OVERFLOW, 1);
      res &= 0x7fff;
    }
  return sc_accumulatorSet (res);
}

static inline int
com_DIV (int operand)
{
  int a, b, res;
  sc_accumulatorGet (&a);
  sc_memoryGet (operand, &b, 1);
  if (!b)
    {
      sc_regSet (FLAG_DIVZERO, 1);
      return -1;
    }
  res = a / b;
  if (res < -0x7FFF || res > 0x7FFF)
    {
      sc_regSet (FLAG_OVERFLOW, 1);
      return -1;
    }
  return sc_accumulatorSet (res);
}

static inline int
com_MUL (int operand)
{
  int a, b, res;
  sc_accumulatorGet (&a);
  sc_memoryGet (operand, &b, 1);

  res = a * b;
  if (res < -0x8000 || res > 0x7FFF)
    {
      sc_regSet (FLAG_OVERFLOW, 1);
      return -1;
    }
  return sc_accumulatorSet (res);
}

// ALU FUNC END

// TRANSFER FUNC START

static inline int
op_JUMP (int operand)
{
  temp = operand - 1;
  return sc_icounterSet (operand);
}

static inline int
op_JNEG (int operand)
{
  int a;
  sc_accumulatorGet (&a);
  if (a >> 14){
    temp = operand - 1;
    return sc_icounterSet (operand);
  }
  return 0;
}

static inline int
op_JZ (int operand)
{
  int a;
  sc_accumulatorGet (&a);
  if (a == 0)
    return sc_icounterSet (operand);
  return 0;
}

static inline int
op_HALT (int operand)
{
  (void)operand;
  temp-=1;
  return sc_regSet (FLAG_IGNORE, 1);
}

// TRANSFER FUNC END

// USER FUNC START

static inline int
op_ADDC (int operand)
{
  int value;
  int m1, m2;
  sc_accumulatorGet(&value);
  sc_memoryGet(value, &m1, 0);
  sc_memoryGet(m1, &m2, 0);
  sc_memoryGet(operand, &m1, 0);
  m1 = m1 + m2;
  return sc_accumulatorSet(m1);
}

static inline int
op_MOVCR (int operand)
{
  int value;
  int m1, m2;
  sc_accumulatorGet(&value);
  sc_memoryGet(value, &m1, 0);
  sc_memoryGet(m1, &m2, 0);
  return sc_memorySet(operand, m2);
}

// USER FUNC END

static void
init_commands (void)
{
  commands[0x00] = op_NOP;
  commands[0x01] = op_CPUINFO;
  commands[0x0A] = op_READ;
  commands[0x0B] = op_WRITE;
  commands[0x14] = op_LOAD;
  commands[0x15] = op_STORE;

  commands[0x1E] = com_ADD;
  commands[0x1F] = com_SUB;
  commands[0x20] = com_DIV;
  commands[0x21] = com_MUL;

  commands[0x28] = op_JUMP;
  commands[0x29] = op_JNEG;
  commands[0x2A] = op_JZ;
  commands[0x2B] = op_HALT;

  commands[0x4A] = op_MOVCR;
  commands[0x4b] = op_ADDC;
}

int
execute (int command, int operand)
{
  if (!initialized)
    {
      init_commands ();
      initialized = 1;
    }

  if (command >= 0 && command < 128 && commands[command])
    {
      return commands[command](operand);
    }
  else
    {
      sc_regSet (FLAG_INVALIDCMD, 1);
      return -1;
    }
  return 0;
}

void
CU (void)
{
  int val, sign, cmd, op;
  sc_icounterGet (&val);
  temp = val;
  sc_memoryGet (val, &val, 1);
  if (sc_commandDecode (val, &sign, &cmd, &op) != 0)
    return;

  if (cmd < 0 || cmd > 0x7F)
    return;

  execute (cmd, op);
}

void
nextTick (void)
{
  int t;
  sc_regGet (FLAG_IGNORE, &t);
  if (!t)
    {
      CU ();
      sc_icounterSet ((temp + 1) % 128);
    }
}

void
IRC (int signum)
{
  int t, val;
  sc_icounterGet (&val);
  temp = val;
  sc_regGet (FLAG_IGNORE, &t);
  if (signum == SIGALRM)
    {
      if (!t)
        {
          CU ();
          sc_icounterSet ((temp + 1) % 128);
        }
    }
  else if (signum == SIGUSR1)
    {
      nextTick ();
      printInterface ();
    }
  else if (signum == SIGINT)
    {
      stopExecutionMode ();
      mt_clrscr ();
      mt_gotoXY (1, 1);
      printf ("*** Прерывание: выполнение остановлено пользователем ***\n");
    }
}

void
initInterrupts (void)
{
  struct sigaction sa;
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = IRC;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction (SIGALRM, &sa, NULL) == -1)
    {
      perror ("sigaction SIGALRM");
      exit (1);
    }

  if (sigaction (SIGUSR1, &sa, NULL) == -1)
    {
      perror ("sigaction SIGUSR1");
      exit (1);
    }
  if (sigaction (SIGINT, &sa, NULL) == -1)
    {
      perror ("sigaction SIGINT");
      exit (1);
    }
}

void
startExecutionMode (void)
{
  sc_regSet (FLAG_IGNORE, 0);
  struct itimerval iv = { { 0, timeout }, { 0, timeout } };
  setitimer (ITIMER_REAL, &iv, NULL);
}

void
stopExecutionMode (void)
{
  sc_regSet (FLAG_IGNORE, 1);
  struct itimerval iv = { { 0, 0 }, { 0, 0 } };
  setitimer (ITIMER_REAL, &iv, NULL);
  sc_regSet (FLAG_IGNORE, 1);
}
