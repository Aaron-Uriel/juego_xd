#include <wchar.h>

#include "ncurses_utils.h"

void draw_window_borders(WINDOW *window) {
    uint32_t height, width;
    getmaxyx(window, height, width);

    // Bordes
    mvwaddstr(window, 0, 0, "┏");
    mvwaddstr(window, height - 1, 0, "┗");
    mvwaddstr(window, 0, width - 1, "┓");
    mvwaddstr(window, height - 1, width - 1, "┛");

    // Lados
    uint32_t i;
    for (i = 1; i < (height - 1); i++) {
        mvwprintw(window, i, 0, u8"┃");
        mvwprintw(window, i, width - 1, u8"┃");
    }
    for (i = 1; i < (width - 1); i++) {
        mvwprintw(window, 0, i, u8"━");
        mvwprintw(window, height - 1, i, u8"━");
    }
}