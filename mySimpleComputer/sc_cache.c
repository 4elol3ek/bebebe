#include "mySimpleComputer.h"
#include "myTerm.h"
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

extern int memory[128];

static CacheLine cache[CACHE_LINES];
static unsigned long cycle_count = 0;

void cacheInit(void) {
    cycle_count = 0;
    for (int i = 0; i < CACHE_LINES; ++i) {
        cache[i].valid = 0;
        cache[i].dirty = 0;
        cache[i].tag   = -1;
        cache[i].age   = 0;
    }
}

unsigned long cacheGetCycles(void) {
    return cycle_count;
}

/* Увеличить возраст всех загруженных строк, обнулить для idx */
static void updateAges(int idx) {
    for (int i = 0; i < CACHE_LINES; ++i)
        if (cache[i].valid)
            cache[i].age++;
    cache[idx].age = 0;
}

/* Найти индекс свободной или LRU-строки */
static int selectLine(void) {
    for (int i = 0; i < CACHE_LINES; ++i)
        if (!cache[i].valid)
            return i;
    int idx = 0;
    unsigned int max_age = cache[0].age;
    for (int i = 1; i < CACHE_LINES; ++i) {
        if (cache[i].age > max_age) {
            max_age = cache[i].age;
            idx = i;
        }
    }
    return idx;
}

/* Размер блока: для последнего блока может быть меньше LINE_SIZE */
static int blockSize(int block) {
    int start = block * LINE_SIZE;
    int remaining = MEM_SIZE - start;
    return (remaining < LINE_SIZE ? remaining : LINE_SIZE);
}

int cacheRead(int address, int *value) {
    if (address < 0 || address >= MEM_SIZE) {
        sc_regSet(FLAG_OUTOFRANGE, 1);
        return -1;
    }
    int block = address / LINE_SIZE;
    int offset = address % LINE_SIZE;
    /* поиск по тегу */
    for (int i = 0; i < CACHE_LINES; ++i) {
        if (cache[i].valid && cache[i].tag == block) {
            cycle_count += 2;
            updateAges(i);
            *value = cache[i].data[offset];
            return 0;
        }
    }
    /* промах */
    cycle_count += 10;
    int idx = selectLine();
    /* write-back */
    if (cache[idx].valid && cache[idx].dirty) {
        int old_block = cache[idx].tag;
        int size = blockSize(old_block);
        int base_old = old_block * LINE_SIZE;
        for (int j = 0; j < size; ++j)
            memory[base_old + j] = cache[idx].data[j];
    }
    /* загрузка новой строки */
    int size = blockSize(block);
    int base = block * LINE_SIZE;
    for (int j = 0; j < size; ++j)
        cache[idx].data[j] = memory[base + j];
    cache[idx].valid = 1;
    cache[idx].dirty = 0;
    cache[idx].tag   = block;
    updateAges(idx);
    *value = cache[idx].data[offset];
    return 0;
}

int cacheWrite(int address, int value) {
    if (address < 0 || address >= MEM_SIZE) {
        sc_regSet(FLAG_OUTOFRANGE, 1);
        return -1;
    }
    int block = address / LINE_SIZE;
    int offset = address % LINE_SIZE;
    for (int i = 0; i < CACHE_LINES; ++i) {
        if (cache[i].valid && cache[i].tag == block) {
            cache[i].data[offset] = value;
            cache[i].dirty = 1;
            cycle_count += 2;
            updateAges(i);
            return 0;
        }
    }
    /* промах: загрузка */
    cycle_count += 10;
    int idx = selectLine();
    if (cache[idx].valid && cache[idx].dirty) {
        int old_block = cache[idx].tag;
        int size_old = blockSize(old_block);
        int base_old = old_block * LINE_SIZE;
        for (int j = 0; j < size_old; ++j)
            memory[base_old + j] = cache[idx].data[j];
    }
    int size = blockSize(block);
    int base = block * LINE_SIZE;
    for (int j = 0; j < size; ++j)
        cache[idx].data[j] = memory[base + j];
    cache[idx].valid = 1;
    cache[idx].tag   = block;
    /* запись */
    cache[idx].data[offset] = value;
    cache[idx].dirty = 1;
    updateAges(idx);
    return 0;
}