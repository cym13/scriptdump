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

void printHelp(FILE* output) {
    fputs("Strip a file of its ansi code\n",             output);
    fputs("\n",                                          output);
    fputs("Usage: scriptdump -h\n",                      output);
    fputs("       scriptdump [-f] FILE\n",               output);
    fputs("\n",                                          output);
    fputs("Arguments:\n",                                output);
    fputs("    FILE    The file to be stripped off.\n",  output);
    fputs("            '-' means standard input\n",      output);
    fputs("\n",                                          output);
    fputs("Options:\n",                                  output);
    fputs("    -h, --help   Print this help and exit\n", output);
    fputs("    -f, --force  Ignore parsing errors\n",    output);
}

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

    long p[8];

    memset(p, -1, 8);

    if (!csiParam(sb, p))
        return false;

    switch (LOOK) {
        case 'A':
            match('A');
            if (p[0] == -1)
                p[0] = 1;

            for (size_t i=0 ; i<p[0] ; i++)
                if (sb->data[sb->line-i][sb->col] == '\0')
                    sb->data[sb->line-i][sb->col] = ' ';

            sbMoveBy(sb, UP, p[0]);
            break;

        case 'B':
            match('B');
            if (p[0] == -1)
                p[0] = 1;

            for (size_t i=0 ; i<p[0] ; i++)
                if (sb->data[sb->line+i][sb->col] == '\0')
                    sb->data[sb->line+i][sb->col] = ' ';

            sbMoveBy(sb, DOWN, p[0]);
            break;

        case 'C':
            match('C');
            if (p[0] == -1)
                p[0] = 1;

            for (size_t i=0 ; i<p[0] ; i++)
                if (sb->data[sb->line][sb->col+i] == '\0')
                    sb->data[sb->line][sb->col+i] = ' ';

            sbMoveBy(sb, RIGHT, p[0]);
            break;

        case 'D':
            match('D');
            if (p[0] == -1)
                p[0] = 1;

            for (size_t i=0 ; i<p[0] ; i++)
                if (sb->data[sb->line][sb->col-i] == '\0')
                    sb->data[sb->line][sb->col-i] = ' ';

            sbMoveBy(sb, LEFT, p[0]);
            break;

        case 'E':
            match('E');
            if (p[0] == -1)
                p[0] = 1;
            sbMoveBy(sb, DOWN, p[0]);
            sb->col = 0;
            break;

        case 'F':
            match('F');
            if (p[0] == -1)
                p[0] = 1;
            sbMoveBy(sb, UP, p[0]);
            sb->col = 0;
            break;

        case 'G':
            match('G');
            if (p[0] == -1)
                p[0] = 1;
            sb->col = p[0];
            break;

        case 'f':
        case 'H':
            match(LOOK);
            if (p[0] == -1)
                p[0] = 1;
            if (p[1] == -1)
                p[1] = 1;
            sbMoveTo(sb, p[0], p[1]);
            break;

        case 'J':
            match('J');
            break;

        case 'K':
            match('K');
            size_t start = (p[0] == 1 || p[0] == 2)  ? 0 : sb->col;
            size_t end   =  p[0] == 1 ? sb->col : SB_MAX_WIDTH-1;
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

        case 'r':
            match('r');
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

bool csiParam(ScreenBuffer sb, long* params) {
    if (!sb)
        return false;

    if (isAnsiTrailChar(LOOK))
        return true;

    char   buf[MAX_ANSI_PARAM_LENGTH];
    size_t buflen = 0;

    if (LOOK == '?')
        match('?');

    for (int i=0 ; i<4 ; i++) {
        while (LOOK >= '0' && LOOK <= '9') {
            buf[buflen] = LOOK;
            buflen++;

            if (buflen > MAX_ANSI_PARAM_LENGTH)
                error("parameter too long");

            match(LOOK);
        }

        params[i] = strtol(buf, NULL, 10);

        if (isAnsiTrailChar(LOOK))
            return true;

        match(';');
    }
    return true;
}

// TODO: stop ignoring thoses codes
bool nonCsiCode(ScreenBuffer sb) {
    if (!sb)
        return false;

    match(LOOK);
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
            sbMove(sb, DOWN);
            return true;

        case '\r':
            match('\r');
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
        case 'n': case 'r': case 's': case 'u': case 'l': case 'h':
            return true;
        default:
            return false;
    }
}

void feedInput(char* fname) {
    FILE* typescript;

    if (strcmp(fname, "-") == 0) {
        typescript = stdin;
    }
    else {
        typescript = fopen(fname, "r");

        if (!typescript)
            error("cannot open the given file");

        acPush(autocleaner, AC_FUNC(fclose), typescript);
    }

    struct stat buf;

    if (fstat(fileno(typescript), &buf))
        error("cannot stat file");

    size_t fsize = buf.st_size;
    acPop(autocleaner);

    if (fsize == 0)
        fsize = MAX_STDIN_INPUT;

    INPUT = malloc((fsize + 1) * sizeof(char));

    if (!INPUT)
        error("out of memory");

    acPush(autocleaner, AC_FUNC(free), INPUT);

    fsize = fread(INPUT, sizeof(char), fsize, typescript);

    if (typescript) {
        fclose(typescript);
        acPop(autocleaner);
    }

    INPUT[fsize] = EOF;

    LOOK = INPUT[0];
}

// TODO: better argument management
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printHelp(stderr);
        return 1;
    }

    if (!strcmp(argv[1], "-h") || (!strcmp(argv[1], "--help"))) {
        printHelp(stdout);
        return 0;
    }

    bool forceOutput = false;

    if (!strcmp(argv[1], "-f") || (!strcmp(argv[1], "--force"))) {
        forceOutput = true;
        argv[1] = argv[2];
    }

    acInit(autocleaner);

    feedInput(argv[1]);

    ScreenBuffer sb = calloc(1, sizeof(struct ScreenBuffer));

    if (!sbInit(sb))
        error("initialization failure");

    acPush(autocleaner, AC_FUNC(sbClean), sb);

    if (!text(sb) && !forceOutput)
        error("parsing failed");

    for (size_t line = 0 ; line < SB_MAX_HEIGHT ; line++) {
        if (sb->data[line][0] == '\0')
            continue;
        printf("%s\n", sb->data[line]);
    }

    sbClean(sb);
    acPop(autocleaner);

    free(INPUT);
    acPop(autocleaner);

    return 0;
}
