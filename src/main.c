#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#include <ncurses.h>

#include "game.h"
#include "resolution.h"
#include "colors.h"

struct Resolution terminal_resolution = {
    .height = 22,
    .width = 80
};

#define OPTION_LIMIT 3
enum Options {
    NEW_GAME = 1,
    CONTINUE_GAME = 2,
    EXIT = 3
};

int main() {
    // Inicialización y ajustes de ncurses
    setlocale(LC_CTYPE, "");
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, terminal_resolution.height, terminal_resolution.width);
    start_color();
    use_default_colors();
    set_color_pairs();

    srand(time(NULL));

    if (has_colors() == FALSE) {
        endwin();
        printf("Tu terminal no soporta colores\n");
        exit(1);
    }

    const char option_list[OPTION_LIMIT][20] = {
        "Nueva partida.",
        "Continuar partida.",
        "Salir."
    };

    char selected_option = 1, i;
    bool is_option_selected, is_enter_pressed = false;
    int32_t input;
    do {
        clear();
        printw("Bienvenido al juego xd.\n"
               u8"¿Qué desea hacer?:\n");

        for (i = 0; i < OPTION_LIMIT; ++i) {
            is_option_selected = ((i + 1) == selected_option);
            printw("    %d.- %-20s%s\n", i + 1, option_list[i], (is_option_selected)? "[*]": "[ ]");
        }

        input = getch();
        switch (input) {
            case KEY_UP: case 'w':
                if (selected_option > 1) {
                    selected_option--;
                }
                break;
            case KEY_DOWN: case 's':
                if (selected_option < 3) {
                    selected_option++;
                }
                break;
            case '\n':
                is_enter_pressed = true;
        }
        if (is_enter_pressed == false) {
            continue;
        }
        switch (selected_option) {
            case NEW_GAME:
                new_game();
                break;
        }
    } while(!(selected_option == 3 && is_enter_pressed == true));

    delwin(stdscr);
    endwin();
    return 0;
}
