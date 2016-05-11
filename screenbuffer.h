#ifndef SCREENBUFFER_H
#define SCREENBUFFER_H

#define SB_MAX_WIDTH  1024
#define SB_MAX_HEIGHT 30

typedef struct ScreenBuffer* ScreenBuffer;
struct ScreenBuffer {
    size_t line;
    size_t col;
    size_t saved_line;
    size_t saved_col;

    char data[SB_MAX_HEIGHT][SB_MAX_WIDTH];
};

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

bool sbInit(ScreenBuffer sb);
bool sbMove(ScreenBuffer sb, Direction d);
bool sbMoveBy(ScreenBuffer sb, Direction d, size_t num);
bool sbMoveTo(ScreenBuffer sb, size_t l, size_t c);
bool sbPut(ScreenBuffer sb, char c);
bool sbSavePos(ScreenBuffer sb);
bool sbRestorePos(ScreenBuffer sb);
void sbClean(ScreenBuffer sb);

#endif
