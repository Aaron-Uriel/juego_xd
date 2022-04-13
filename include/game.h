#ifndef GAME_H
#define GAME_H

typedef struct {
    uint8_t height;
    uint8_t width;
} Resolution;
extern Resolution terminal_resolution;
void new_game();

#endif