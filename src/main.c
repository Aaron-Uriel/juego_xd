#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>

#include "pseudoconio.h"
#include "game.h"

#define UP_ARROW    "\033[A"
#define DOWN_ARROW  "\033[B"
#define LEFT_ARROW  "\033[D"
#define RIGHT_ARROW "\033[C"

typedef struct {
    uint8_t height;
    uint8_t width;
} Resolution;
extern const Resolution terminal_resolution = {
    .height = 23,
    .width = 80
};

#define OPTION_LIMIT 3
enum Options {
    NEW_GAME = 1,
    CONTINUE_GAME = 2,
    EXIT = 3
};

int main() {
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
            case 'A': 
                if (selected_option > 1) {
                    selected_option--;
                }
                break;
            case 'B': 
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