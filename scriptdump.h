#ifndef SCRIPTDUMP_H
#define SCRIPTDUMP_H

#include "screenbuffer.h"
#include "autocleaner.h"

#define error(msg)                                          \
    do {                                                    \
        fprintf(stderr, "0x%x Error: %s\n", INPUT_POS, msg); \
        safeExit();                                         \
    }while(0)

#define expect(msg)                                              \
    do {                                                         \
        fprintf(stderr, "0x%x Expecting '%s'\n", INPUT_POS, msg); \
        safeExit();                                              \
    }while(0)

#define expectChar(chr)                                          \
    do {                                                         \
        fprintf(stderr, "0x%x Expecting '%c'\n", INPUT_POS, chr); \
        safeExit();                                              \
    }while(0)

#define MAX_ANSI_PARAM_LENGTH 32

void match(char c);

/*
 * text        := (ansiCode | specialChar | rawChar)* | $
 * ansiCode    := \x1B (csiCode | nonCsiCode)
 * csiCode     := \[ csiParam [ABCDEFGHJKSTfmnsulh]
 * csiParam    := ( \??[0-9]+ | [0-9]+;[0-9]+ )
 * nonCsiCode  := [NO^_P\]c=]
 * specialChar := [\b\n\r]
 * rawChar     := Anything else
 */

bool text(ScreenBuffer sb);
bool ansiCode(ScreenBuffer sb);
bool csiCode(ScreenBuffer sb);
bool csiParam(ScreenBuffer sb, long* param);
bool nonCsiCode(ScreenBuffer sb);
bool specialChar(ScreenBuffer sb);
bool rawChar(ScreenBuffer sb);

bool isSpecialChar(char c);
bool isAnsiTrailChar(char c);

void feedInput(char* fname);

#endif
