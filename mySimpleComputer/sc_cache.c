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

extern CacheLine cache[CACHE_LINES];
unsigned long cycle_count = 0;

void sc_cacheInit(void) {
    cycle_count = 0;
    // mt_gotoXY(27,1);
    // printf("%p in init\n", (void*)cache);
    // fflush(stdout);
    memset(cache, 0, sizeof(cache));
}

unsigned long cacheGetCycles(void) {
    return cycle_count;
}

void updateAges(int idx) {
    for (int i = 0; i < CACHE_LINES; ++i)
        if (cache[i].valid)
            cache[i].age++;
    cache[idx].age = 0;
}

int selectLine(void) {
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

int lineSize(int tag) {
    return (tag == 12 ? 8 : 10);
}

int sc_cacheRead(int address, int *value) {
    if (address < 0 || address >= MEM_SIZE) {
        sc_regSet(FLAG_OUTOFRANGE, 1);
        return -1;
    }
    int tag = address / LINE_SIZE;
    int offset = address % LINE_SIZE;

    for (int i = 0; i < CACHE_LINES; ++i) {
        if (cache[i].valid && cache[i].tag == tag) {
            cycle_count += 2;
            updateAges(i);
            *value = cache[i].data[offset];
            printf("cache-hit\n");
            fflush(stdout);
            return 0; // cache-hit
        }
    }
    /* промах */
    cycle_count += 10;
    int idx = selectLine();
    /* write-back */
    if (cache[idx].valid && cache[idx].dirty) {
        int old_tag = cache[idx].tag;
        int size = lineSize(old_tag);
        int base_old = old_tag * LINE_SIZE;
        for (int j = 0; j < size; ++j)
            memory[base_old + j] = cache[idx].data[j];
    }

    int size = lineSize(tag);
    int base = tag * LINE_SIZE;
    for (int j = 0; j < size; ++j)
        cache[idx].data[j] = memory[base + j];
    cache[idx].valid = 1;
    cache[idx].dirty = 0;
    cache[idx].tag = tag;
    updateAges(idx);
    *value = cache[idx].data[offset];
    return 0;
}

int sc_cacheWrite(int address, int value) {
    if (address < 0 || address >= MEM_SIZE) {
        sc_regSet(FLAG_OUTOFRANGE, 1);
        return -1;
    }
    int tag = address / LINE_SIZE;
    int offset = address % LINE_SIZE;
    for (int i = 0; i < CACHE_LINES; ++i) {
        if (cache[i].valid && cache[i].tag == tag) {
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
        int old_tag = cache[idx].tag;
        int size_old = lineSize(old_tag);
        int base_old = old_tag * LINE_SIZE;
        for (int j = 0; j < size_old; ++j)
            memory[base_old + j] = cache[idx].data[j];
    }
    int size = lineSize(tag);
    int base = tag * LINE_SIZE;
    for (int j = 0; j < size; ++j)
        cache[idx].data[j] = memory[base + j];
    cache[idx].valid = 1;
    cache[idx].tag   = tag;
    /* запись */
    cache[idx].data[offset] = value;
    cache[idx].dirty = 1;
    updateAges(idx);
    return 0;
}