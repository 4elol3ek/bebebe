#include "mySimpleComputer.h"
#include "myTerm.h"
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define ROTL15(x, n) ((((x) << (n)) | ((x) >> (15 - (n)))) & 0x7FFF)
#define ROTR15(x, n) ((((x) >> (n)) | ((x) << (15 - (n)))) & 0x7FFF)

int timeout = 250000;
static int temp;

extern int printInterface (void);
static int (*inout_com[128]) (int operand) = { NULL };
static int (*ALU_com[128]) (int operand) = { NULL };
static int (*trans_com[128]) (int operand) = { NULL };
static int (*user_com[128]) (int operand) = { NULL };
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
  printf ("ИА-331 Шкляев Денис Викторович\nИА-331 Иргит Сенгин Хурешович");
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
  printf ("                                               \n                  "
          "                         ");
  mt_gotoXY (27, 1);
  sc_memoryGet (operand, &a, 0);
  printf ("Ячейка №%d hex: %04X", operand + 1, a);
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
  if (res < -0x3FFF || res > 0x7FFF)
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

  res = a / b;
  if (res < -0x3FFF || res > 0x3FFF)
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
  if (res < -0x3FFF || res > 0x3FFF)
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
  if (a >> 14)
    return sc_icounterSet (operand);
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
  return sc_regSet (FLAG_IGNORE, 1);
}

// TRANSFER FUNC END

// USER FUNC START

static inline int
op_NOT (int operand)
{
  int a;
  sc_accumulatorGet (&a);
  invers (&a);
  sc_memorySet (operand, a);
  return 0;
}

static inline int
op_AND (int operand)
{
  int a, b;
  sc_accumulatorGet (&a);
  sc_memoryGet (operand, &b, 1);
  return sc_accumulatorSet ((a & b) & 0x7FFF);
}

static inline int
op_RCR (int operand)
{
  int b;
  sc_memoryGet (operand, &b, 0);
  return sc_accumulatorSet (ROTR15 (b, 1));
}

static inline int
op_NEG (int operand)
{
  int b;
  sc_memoryGet (operand, &b, 0);
  if ((b >> 14))
    {
      invers (&b);
      b += 1;
      b |= 0x4000;
    }
  return sc_accumulatorSet (b & 0x7FFF);
}

// USER FUNC END

static void
init_commands (void)
{
  inout_com[0x00] = op_NOP;
  inout_com[0x01] = op_CPUINFO;
  inout_com[0x0A] = op_READ;
  inout_com[0x0B] = op_WRITE;
  inout_com[0x14] = op_LOAD;
  inout_com[0x15] = op_STORE;

  ALU_com[0x1E] = com_ADD;
  ALU_com[0x1F] = com_SUB;
  ALU_com[0x20] = com_DIV;
  ALU_com[0x21] = com_MUL;

  trans_com[0x28] = op_JUMP;
  trans_com[0x29] = op_JNEG;
  trans_com[0x2A] = op_JZ;
  trans_com[0x2B] = op_HALT;

  user_com[0x33] = op_NOT;
  user_com[0x34] = op_AND;
  user_com[0x3F] = op_RCR;
  user_com[0x40] = op_NEG;
}

int
inout (int command, int operand)
{
  if (!initialized)
    {
      init_commands ();
      initialized = 1;
    }

  if (command >= 0 && command < 128 && inout_com[command])
    {
      return inout_com[command](operand);
    }
  else
    {
      sc_regSet (FLAG_INVALIDCMD, 1);
      return -1;
    }
  return 0;
}

int
ALU (int command, int operand)
{
  if (!initialized)
    {
      init_commands ();
      initialized = 1;
    }

  if (command >= 0 && command < 128 && ALU_com[command])
    {
      return ALU_com[command](operand);
    }
  else
    {
      sc_regSet (FLAG_INVALIDCMD, 1);
      return -1;
    }
  return 0;
}

int
trans (int command, int operand)
{
  if (!initialized)
    {
      init_commands ();
      initialized = 1;
    }

  if (command >= 0 && command < 128 && trans_com[command])
    {
      return trans_com[command](operand);
    }
  else
    {
      sc_regSet (FLAG_INVALIDCMD, 1);
      return -1;
    }
  return 0;
}

int
userfunc (int command, int operand)
{
  if (!initialized)
    {
      init_commands ();
      initialized = 1;
    }

  if (command >= 0 && command < 128 && user_com[command])
    {
      return user_com[command](operand);
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

  if (cmd < 0x15)
    inout (cmd, op);
  else if (cmd == 0x15)
    trans (cmd, op);
  else if (cmd > 0x15 && cmd <= 0x21)
    ALU (cmd, op);
  else if (cmd > 0x21 && cmd <= 0x2B)
    trans (cmd, op);
  else if (cmd > 0x2B && cmd <= 0x40)
    userfunc (cmd, op);
}

void
nextTick (void)
{
  int ic;
  sc_icounterGet (&ic);

  int t;
  sc_regGet (FLAG_IGNORE, &t);
  if (!t)
    {
      CU ();
      sc_icounterSet ((ic + 1) % 128);
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
