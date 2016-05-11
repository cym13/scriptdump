#include <stdbool.h>
#include "autocleaner.h"

bool acInit(AutoCleaner ac) {
    if (!ac)
        return false;

    ac->current = 0;
    return true;
}

bool acPush(AutoCleaner ac, void f(), void* args) {
    if (!ac || ac->current == MAX_CLEANERS-1)
        return false;

    ac->current++;
    ac->stack[ac->current] = f;
    ac->args[ac->current]  = args;
    return true;
}

bool acPop(AutoCleaner ac) {
    if (!ac || ac->current == 0)
        return false;
    ac->current--;
    return true;
}

bool acClean(AutoCleaner ac) {
    if (!ac)
        return false;

    for (int i=0 ; i<ac->current ; i++)
        ((void (*)(void*))(ac->stack[i]))(ac->args[i]);
}
