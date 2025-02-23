#ifndef MYSIMPLECOMPUTER_H
#define MYSIMPLECOMPUTER_H

#define FLAG_OVERFLOW 0x1
#define FLAG_DIVZERO 0x2
#define FLAG_OUTOFRANGE 0x4
#define FLAG_IGNORE 0x8

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

/* Команды */
int sc_commandEncode (int sign, int command, int operand, int *value);
int sc_commandDecode (int val, int *sign, int *command, int *operand);

#endif
