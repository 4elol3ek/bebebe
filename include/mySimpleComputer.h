#ifndef MYSIMPLECOMPUTER_H
#define MYSIMPLECOMPUTER_H

#define FLAG_INVALIDCMD 0x1
#define FLAG_IGNORE 0x2
#define FLAG_OVERFLOW 0x4
#define FLAG_DIVZERO 0x8
#define FLAG_OUTOFRANGE 0x10

#define INOUT_SIZE 5

#define CACHE_LINES 5
#define LINE_SIZE 10

#define TIMEOUT 100000

#define MEM_SIZE 128

/* Функции памяти */
int sc_memoryInit (void);
int sc_memorySet (int address, int value);
int sc_memorySetE (int address, int value);
int sc_memoryGet (int address, int *value);
int sc_memoryPeek (int address, int *value);
int sc_memorySave (const char *filename);
int sc_memoryLoad (const char *filename);
void sc_editcurrentcell (int address, int interactive);

/* Функции регистров */
int sc_regInit (void);
int sc_regSet (int flag, int value);
int sc_regGet (int flag, int *value);
int sc_accumulatorInit (void);
int sc_accumulatorSet (int value);
int sc_accumulatorGet (int *value);
int sc_icounterInit (void);
int sc_icounterSet (int value);
int sc_icounterGet (int *value);

/* Команды */
int sc_commandEncode (int sign, int command, int operand, int *value);
int sc_commandDecode (int value, int *sign, int *command, int *operand);
void inoutAdd (int address, char type, int value);
void sc_editaccumulator (void);
void sc_editicounter (int *cell);
void invers (int *value);
int isvalidcommands ();

/*Функции процессора*/
void initInterrupts (void);
void startExecutionMode (void);
void stopExecutionMode (void);

/*Кэш*/

void sc_cacheInit (void);
int sc_cacheRead (int address, int *value);
int sc_cacheWrite (int address, int value);
unsigned long sc_cacheGetCycles (void);
void updateAges (int idx);
int selectLine (void);
int sc_cacheFlush (int line);

typedef struct
{
  int valid;
  int dirty;
  int tag;
  int data[LINE_SIZE];
  unsigned int age;
} CacheLine;

typedef struct
{
  int address;
  char type;
  int value;
} InOutEntry;

#endif