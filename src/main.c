#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#include <ncurses.h>

#include "game.h"
#include "resolution.h"
#include "colors.h"
#include "utils.h"

struct Resolution terminal_resolution = {
    .length = 22,
    .width = 80
};

enum Options {
    NEW_GAME,
    CONTINUE_GAME,
    EXIT,
    OPTIONS_LIMIT
};

int main() {
    // Inicialización y ajustes de ncurses
    setlocale(LC_CTYPE, "");
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    getmaxyx(stdscr, terminal_resolution.length, terminal_resolution.width);
    start_color();
    use_default_colors();
    set_color_pairs();

    srand(time(NULL));

    if (has_colors() == FALSE) {
        endwin();
        printf("Tu terminal no soporta colores\n");
        exit(1);
    }

    const struct Resolution menu_resolution = {
        .length = OPTIONS_LIMIT,
        .width  = 30
    };
    const struct Point menu_start_point = {
        .y = (terminal_resolution.length / 2) - (menu_resolution.length/2),
        .x = (terminal_resolution.width / 2)  - (menu_resolution.width/2)
    };
    WINDOW * const menu_border_window = newwin(menu_resolution.length+2, menu_resolution.width+2, menu_start_point.y-1, menu_start_point.x-1);
    WINDOW * const menu_window        = newwin(menu_resolution.length,   menu_resolution.width,   menu_start_point.y,   menu_start_point.x);
    keypad(menu_window, TRUE);

    const uint8_t title_center = (terminal_resolution.width/2) - (67/2);
    mvprintw(1, title_center, "     ██╗██╗   ██╗███████╗ ██████╗  ██████╗         ██╗  ██╗██████╗ ");
    mvprintw(2, title_center, "     ██║██║   ██║██╔════╝██╔════╝ ██╔═══██╗        ╚██╗██╔╝██╔══██╗");
    mvprintw(3, title_center, "     ██║██║   ██║█████╗  ██║  ███╗██║   ██║         ╚███╔╝ ██║  ██║");
    mvprintw(4, title_center, "██   ██║██║   ██║██╔══╝  ██║   ██║██║   ██║         ██╔██╗ ██║  ██║");
    mvprintw(5, title_center, "╚█████╔╝╚██████╔╝███████╗╚██████╔╝╚██████╔╝███████╗██╔╝ ██╗██████╔╝");
    mvprintw(6, title_center, " ╚════╝  ╚═════╝ ╚══════╝ ╚═════╝  ╚═════╝ ╚══════╝╚═╝  ╚═╝╚═════╝ ");
    refresh();

    draw_window_borders(menu_border_window);
    wrefresh(menu_border_window);

    uint8_t selected_option = 0;
    bool is_enter_pressed = false;
    int32_t input;
    do {
        mvwprintw(menu_window, NEW_GAME,      0, " 1.- %-18s [ ]", "Nuevo juego.");
        mvwprintw(menu_window, CONTINUE_GAME, 0, " 2.- %-18s [ ]", "Continuar juego.");
        mvwprintw(menu_window, EXIT,          0, " 3.- %-18s [ ]", "Salir.");

        mvwaddch(menu_window, selected_option, 25, '*');

        input = wgetch(menu_window);
        mvwaddch(menu_window, selected_option, 25, ' ');
        switch (input) {
            case KEY_UP:   case 'w': --selected_option; break;
            case KEY_DOWN: case 's': ++selected_option; break;
            case '\n':     is_enter_pressed = true;
        }
        if (selected_option > OPTIONS_LIMIT - 1) { selected_option = 0; }
        if (is_enter_pressed == true) {
            switch (selected_option) {
                case NEW_GAME: new_game(); break;
                case EXIT:
                    delwin(menu_border_window);
                    delwin(menu_window);
                    endwin();
                    return 0;
            }
        }
    } while(true);

    return 0;
}
