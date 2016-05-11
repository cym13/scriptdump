#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "scriptdump.h"
#include "autocleaner.h"
#include "screenbuffer.h"

static char   LOOK;
static char*  INPUT;
static size_t INPUT_POS = 0;

static AutoCleaner autocleaner;

void match(char c) {
    if (LOOK != c)
        expectChar(c);

    INPUT_POS++;
    LOOK = INPUT[INPUT_POS];
}

bool text(ScreenBuffer sb) {
    if (!sb)
        return false;

    if (LOOK == EOF)
        return true;

    bool retval = true;

    if (LOOK == '\x1B')
        retval &= ansiCode(sb);

    else if (isSpecialChar(LOOK))
        retval &= specialChar(sb);

    else
        retval &= rawChar(sb);

    if (!retval)
        return false;

    return text(sb);
}

bool ansiCode(ScreenBuffer sb) {
    if (!sb)
        return false;

    match('\x1B');

    if (LOOK == '[')
        return csiCode(sb);

    return nonCsiCode(sb);
}

bool csiCode(ScreenBuffer sb) {
    if (!sb)
        return false;

    match('[');

    long n = -1;
    long m = -1;

    if (!csiParam(sb, &n, &m))
        return false;

    switch (LOOK) {
        case 'A':
            match('A');
            if (n == -1)
                n = 1;

            for (size_t i=0 ; i<n ; i++)
                if (sb->data[sb->line-i][sb->col] == '\0')
                    sb->data[sb->line-i][sb->col] = ' ';

            sbMoveBy(sb, UP, n);
            break;

        case 'B':
            match('B');
            if (n == -1)
                n = 1;

            for (size_t i=0 ; i<n ; i++)
                if (sb->data[sb->line+i][sb->col] == '\0')
                    sb->data[sb->line+i][sb->col] = ' ';

            sbMoveBy(sb, DOWN, n);
            break;

        case 'C':
            match('C');
            if (n == -1)
                n = 1;

            for (size_t i=0 ; i<n ; i++)
                if (sb->data[sb->line][sb->col+i] == '\0')
                    sb->data[sb->line][sb->col+i] = ' ';

            sbMoveBy(sb, RIGHT, n);
            break;

        case 'D':
            match('D');
            if (n == -1)
                n = 1;

            for (size_t i=0 ; i<n ; i++)
                if (sb->data[sb->line][sb->col-i] == '\0')
                    sb->data[sb->line][sb->col-i] = ' ';

            sbMoveBy(sb, LEFT, n);
            break;

        case 'E':
            match('E');
            if (n == -1)
                n = 1;
            sbMoveBy(sb, DOWN, n);
            sb->col = 0;
            break;

        case 'F':
            match('F');
            if (n == -1)
                n = 1;
            sbMoveBy(sb, UP, n);
            sb->col = 0;
            break;

        case 'G':
            match('G');
            if (n == -1)
                n = 1;
            sb->col = n;
            break;

        case 'f':
        case 'H':
            match(LOOK);
            if (n == -1)
                n = 1;
            if (m == -1)
                m = 1;
            sbMoveTo(sb, n, m);
            break;

        case 'J':
            match('J');
            break;

        case 'K':
            match('K');
            size_t start = (n == 1 || n == 2)  ? 0 : sb->col;
            size_t end   =  n == 1 ? sb->col : SB_MAX_WIDTH-1;
            size_t tmp   = sb->col;

            for (sb->col = start ; sb->col < end;) {
                if (!sbPut(sb, '\0'))
                    return false;
            }
            sb->col = tmp;
            break;

        case 'S':
            match('S');
            // TODO
            break;

        case 'T':
            match('T');
            // TODO
            break;

        case 'm':
            match('m');
            // TODO
            break;

        case 'n':
            match('n');
            // TODO
            break;

        case 's':
            match('s');
            sbSavePos(sb);
            break;

        case 'u':
            match('u');
            sbRestorePos(sb);
            break;

        case 'l':
            match('l');
            break;

        case 'h':
            match('h');
            break;

        default:
            return rawChar;
    }
    return true;
}

bool csiParam(ScreenBuffer sb, long* n, long* m) {
    if (!sb)
        return false;

    if (isAnsiTrailChar(LOOK))
        return true;

    char   buf[MAX_ANSI_PARAM_LENGTH];
    size_t buflen = 0;

    if (LOOK == '?')
        match('?');

    while (LOOK >= '0' && LOOK <= '9') {
        buf[buflen] = LOOK;
        buflen++;

        if (buflen > MAX_ANSI_PARAM_LENGTH)
            error("parameter too long");

        match(LOOK);
    }

    *n = strtol(buf, NULL, 10);

    if (isAnsiTrailChar(LOOK))
        return true;

    match(';');

    while (LOOK >= '0' && LOOK <= '9') {
        buf[buflen] = LOOK;
        buflen++;

        if (buflen > MAX_ANSI_PARAM_LENGTH)
            error("parameter too long");

        match(LOOK);
    }

    *m = strtol(buf, NULL, 10);

    return true;
}

// TODO: stop ignoring thoses codes
bool nonCsiCode(ScreenBuffer sb) {
    if (!sb)
        return false;

    switch (LOOK) {
        case 'N':
            match('N');
            break;

        case 'O':
            match('O');
            break;

        case 'P':
            match('P');
            break;

        case 'c':
            match('c');
            break;

        case '^':
            match('^');
            break;

        case '_':
            match('_');
            break;

        case ']':
            match(']');
            break;

        case '=':
            match('=');
            break;

        case '>':
            match('>');
            break;

        default:
            return false;
    }

    return true;
}

bool specialChar(ScreenBuffer sb) {
    if (!sb)
        return false;

    switch (LOOK) {
        case '\b':
            match('\b');
            sbMove(sb, LEFT);
            sbPut(sb, '\0');
            sbMove(sb, LEFT);
            return true;

        case '\n':
            match('\n');
            sbPut(sb, '\n');
            sbMoveTo(sb, sb->line, 0);
            sbMove(sb, DOWN);
            return true;

        case '\r':
            match('\r');
            if (LOOK != '\n')
                sbMoveTo(sb, sb->line, 0);
            return true;
    }
    return false;
}

bool rawChar(ScreenBuffer sb) {
    if (!sb)
        return false;

    if (!sbPut(sb, LOOK))
        return false;

    match(LOOK);
    return true;
}

bool isSpecialChar(char c) {
    switch (c) {
        case '\b': case '\n': case '\r':
            return true;
        default:
            return false;
    }
}

bool isAnsiTrailChar(char c) {
    switch (c) {
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
        case 'H': case 'J': case 'K': case 'S': case 'T': case 'f': case 'm':
        case 'n': case 's': case 'u': case 'l': case 'h':
            return true;
        default:
            return false;
    }
}

void feedInput(char* fname) {
    FILE* typescript = fopen(fname, "r");
    if (!typescript)
        error("cannot open the given file");

    acPush(autocleaner, AC_FUNC(fclose), typescript);

    struct stat buf;

    if (fstat(fileno(typescript), &buf)) {
        error("cannot stat file");
    }

    size_t fsize = buf.st_size;
    acPop(autocleaner);

    INPUT = malloc((fsize + 1) * sizeof(char));

    if (!INPUT) {
        fclose(typescript);
        error("out of memory");
    }

    acPush(autocleaner, AC_FUNC(free), INPUT);

    INPUT[fsize] = EOF;

    if (!fread(INPUT, sizeof(char), fsize, typescript)) {
        error("cannot read file");
    }

    LOOK = INPUT[0];
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        puts("Dump script output with no ansi code left");
        puts("Usage: scriptdump FILE");
        return 1;
    }

    acInit(autocleaner);

    feedInput(argv[1]);

    ScreenBuffer sb = calloc(1, sizeof(struct ScreenBuffer));

    if (!sbInit(sb)) {
        error("initialization failure");
    }

    acPush(autocleaner, AC_FUNC(sbClean), sb);

    if (!text(sb)) {
        error("parsing failed");
    }

    for (size_t line = 0 ; line < SB_MAX_HEIGHT ; line++)
        printf("%s", sb->data[line]);

    sbClean(sb);
    acPop(autocleaner);

    free(INPUT);
    acPop(autocleaner);


    return 0;
}
