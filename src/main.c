#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <assert.h>

#include "pseudoconio.h"
#include "game.h"

#define UP_ARROW    "\033[A"
#define DOWN_ARROW  "\033[B"
#define LEFT_ARROW  "\033[D"
#define RIGHT_ARROW "\033[C"

extern Resolution terminal_resolution;
Resolution terminal_resolution = {
    .height = 24,
    .width = 80
};

void resolution_screen();
void create_spacing_strings(char string1[], char string2[], const uint8_t limit);

#define OPTION_LIMIT 3
enum Options {
    NEW_GAME = 1,
    CONTINUE_GAME = 2,
    EXIT = 3
};

int main() {
    resolution_screen();
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

void resolution_screen() {
    const uint8_t printf_text_height = 4, printf_text_width = 42;
    uint8_t height = printf_text_height, width = printf_text_width, section_limit, horizontal_limit;
    uint8_t row, column;

    char spacing_string1[255], spacing_string2[255], border_string[42];

    char option;
    do {
        system("clear");

        section_limit = (height / 2) - (printf_text_height / 2);
        horizontal_limit = (width / 2);
        for (row = 0; row < section_limit; row++) {
            for (column = 0; column < horizontal_limit; column++) {
                printf(" ");
            }
            printf("#\n");
        }
        assert(row == section_limit);

        section_limit = section_limit + printf_text_height;
        horizontal_limit = (width/2 - printf_text_width/2);
        memset(spacing_string1, ' ', horizontal_limit); spacing_string1[horizontal_limit] = '\0';
        memset(spacing_string2, '#', horizontal_limit); spacing_string2[horizontal_limit] = '\0';
        memset(border_string, '-', printf_text_width);
        printf("%s%s\n", spacing_string1, border_string);
        printf("%s| ¿Cuál es la resolución de su terminal? |\n", spacing_string1);
        printf("%s| Ajuste al centro y pulse Enter         |%s\n", spacing_string2, spacing_string2);
        printf("%s| Altura: %3d, Anchura: %3d              |\n", spacing_string1, height, width);
        printf("%s%s\n", spacing_string1, border_string);

        section_limit = (height / 2) - (printf_text_height / 2);
        horizontal_limit = (width / 2);
        for (row = 0; row < section_limit; row++) {
            for (column = 0; column < horizontal_limit; column++) {
                printf(" ");
            }
            printf("#\n");
        }

        option = getch();
        switch (option) {
            case 'w': case 'A':
                if (height > printf_text_height) { height -= 2; }
                break;
            case 'a': case 'D':
                if (width > printf_text_width) { width -= 2; }
                break;
            case 's': case 'B':
                height += 2;
                break;
            case 'd': case 'C':
                width += 2;
                break;
            case '\n': 
                terminal_resolution = (Resolution) {.height = height, .width = width};
                break;
        }
    } while (option != '\n');
}