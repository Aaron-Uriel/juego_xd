#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pseudoconio.h"
#include "game.h"
#include "resolution.h"

Resolution terminal_resolution = {
    .height = 36,
    .width = 168
};

#define OPTION_LIMIT 3
enum Options {
    NEW_GAME = 1,
    CONTINUE_GAME = 2,
    EXIT = 3
};

int main() {
    //resolution_screen();
    const char option_list[OPTION_LIMIT][20] = {
        "Nueva partida.",
        "Continuar partida.",
        "Salir."
    };

    char selected_option = 1, i;
    bool is_option_selected, is_enter_pressed;
    char input;
    do {
        system("clear");
        printf("Bienvenido al juego xd.\n"
               "¿Qué desea hacer?:\n");

        for (i = 0; i < OPTION_LIMIT; ++i) {
            is_option_selected = ((i + 1) == selected_option);
            printf("    %d.- %-20s%s\n", i + 1, option_list[i], (is_option_selected)? "[*]": "[ ]");
        }

        input = getch();
        switch (input) {
            case 'A': case 'w':
                if (selected_option > 1) {
                    selected_option--;
                }
                break;
            case 'B': case 's':
                if (selected_option < 3) {
                    selected_option++;
                }
                break;
        }
        is_enter_pressed = (input == '\n')? true: false;
        if (is_enter_pressed && selected_option == NEW_GAME) {
            new_game();
        }
    } while(!(selected_option == 3 && is_enter_pressed == true));
    return 0;
}
