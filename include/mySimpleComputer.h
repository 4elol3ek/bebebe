#include "myTerm.h"
#include <stdio.h>

#ifndef MYSIMPLECOMPUTER_H
#define MYSIMPLECOMPUTER_H

#define FLAG_OVERFLOW 0x1
#define FLAG_DIVZERO 0x2
#define FLAG_OUTOFRANGE 0x4
#define FLAG_IGNORE 0x8
#define FLAG_INVALIDCMD 0x10

/* Функции памяти */
int sc_memoryInit (void);
int sc_memorySet (int address, int value);
int sc_memoryGet (int address, int *value);
int sc_memorySave (const char *filename);
int sc_memoryLoad (const char *filename);

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
int sc_commandDecode (int val, int *sign, int *command, int *operand);

/*INOUT Functions*/
void printFlags (void);
void printCell (int address, enum colors fg, enum colors bg);
void printDecodedCommand (int value);
void printAccumulator (void);
void printCounters (void);

#endif