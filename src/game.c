#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "pseudoconio.h"
#include "world.h"
#include "resolution.h"

void render_visible(const World *world, Entity *player);
bool player_move(uint16_t y, uint16_t x, Entity *player, World world);

void new_game() {
    World *world = init_world(100, 200);
    const char (*map)[world->width] = (char(*)[world->width]) world->world_ptr;

    Entity *entities[10];
    Entity *player = init_entity(world, '*');
    entities[0] = player;

    char option;
    do {
        system("clear");
        update_world(world, entities, 1);
        render_visible(world, player);
        option = getch();
        
        int8_t delta_x = 0, delta_y = 0;
        switch (option) {
            case 'w': case 'A':
                delta_y = -1;
                break;
            case 'a': case 'D':
                delta_x = -1;
                break;
            case 's': case 'B':
                delta_y = 1;
                break;
            case 'd': case 'C':
                delta_x = 1;
                break;
        }
        request_change_of_position(delta_x, delta_y, player, world);
    } while(1);
}

void render_visible(const World *world, Entity *player) {
    const char (*map)[world->width] = (char(*)[world->width]) world->world_ptr;

    //Cálculos para saber de dónde a donde se va a poder ver
    uint16_t the_upper_visible_point = player->current_position.y - (terminal_resolution.height/2) - 1;
    uint16_t the_leftmost_visible_point = player->current_position.x - (terminal_resolution.width/2) - 1;

    if (the_upper_visible_point > world->height) {
        the_upper_visible_point = 0;
    }
    if (the_leftmost_visible_point > world->width) {
        the_leftmost_visible_point = 0;
    }

    uint16_t the_lowest_visible_point = the_upper_visible_point + terminal_resolution.height - 1;
    uint16_t the_rightest_visible_point = the_leftmost_visible_point + terminal_resolution.width - 1;

    if (the_lowest_visible_point > world->height) {
        the_lowest_visible_point = world->height - 1;
    }
    if (the_rightest_visible_point > world->width) {
        the_rightest_visible_point = world->width - 1;
    }
    //Fin de los cálculos

    uint16_t row, column;
    for (row = the_upper_visible_point; row <= the_lowest_visible_point; row++) {
        for (column = the_leftmost_visible_point; column <= the_rightest_visible_point; column++) {
            printf("%c", map[row][column]);
        }
        printf("\n");
    }
    printf("P(%d, %d)", player->current_position.y, player->current_position.x);
}