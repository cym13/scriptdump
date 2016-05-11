#ifndef AUTOCLEANER_H
#define AUTOCLEANER_H

#include "stdlib.h"
#include "stdbool.h"

#define MAX_CLEANERS 16
#define AC_FUNC(f) ((void (*)(void*)) &f)

typedef struct AutoCleaner* AutoCleaner;
struct AutoCleaner {
    size_t current;
    void*  stack[MAX_CLEANERS];
    void*  args[MAX_CLEANERS];
};

bool acInit(AutoCleaner ac);
bool acPush(AutoCleaner ac, void f(), void* args);
bool acPop();
bool acClean(AutoCleaner ac);

#define safeExit() do { acClean(autocleaner); exit(1); }while(0)

#endif
