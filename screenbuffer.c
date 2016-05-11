#include <stdlib.h>
#include <stdbool.h>
#include "screenbuffer.h"

bool sbInit(ScreenBuffer sb) {
    if (!sb)
        return false;

    for (int i=0 ; i<SB_MAX_HEIGHT ; i++) {
        for (int j=0 ; j<SB_MAX_WIDTH ;j++) {
            sb->data[i][j] = 0;
        }
    }

    sb->line = 0;
    sb->col  = 0;

    sb->saved_line = 0;
    sb->saved_col  = 0;

    return true;
}

bool sbMove(ScreenBuffer sb, Direction d) {
    if (!sb)
        return false;

    switch (d) {
        case UP:
            if (sb->line == 0)
                return false;
            sb->line--;
            break;

        case DOWN:
            if (sb->line == SB_MAX_HEIGHT-1)
                return false;
            sb->line++;
            break;

        case LEFT:
            if (sb->col == 0)
                return false;
            sb->col--;
            break;

        case RIGHT:
            if (sb->col == SB_MAX_WIDTH-1)
                return false;
            sb->col++;
            break;

        default:
            return false;
    }
    return true;
}

bool sbMoveBy(ScreenBuffer sb, Direction d, size_t num) {
    for (size_t i=0 ; i<num ; i++)
        sbMove(sb, d);
}

bool sbMoveTo(ScreenBuffer sb, size_t l, size_t c) {
    if (!sb)
        return false;

    if ((l >= SB_MAX_HEIGHT) || (c >= SB_MAX_WIDTH))
        return false;

    sb->line = l;
    sb->col  = c;
}

bool sbPut(ScreenBuffer sb, char c) {
    if (!sb)
        return false;

    sb->data[sb->line][sb->col] = c;
    return sbMove(sb, RIGHT);
}

bool sbSavePos(ScreenBuffer sb) {
    sb->saved_line = sb->line;
    sb->saved_col  = sb->col;
}

bool sbRestorePos(ScreenBuffer sb) {
    sb->line = sb->saved_line;
    sb->col  = sb->saved_col;
}

void sbClean(ScreenBuffer sb) {
    return free(sb);
}
