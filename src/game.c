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
    const char (*map)[world->width] = (char(*)[world->width]) world->raw_table;

    char entities_limit = 64;
    Entity *player = init_entity(world, '*');
    Entity *entities[entities_limit];
    entities[0] = player;
    int i;
    for (i = 1; i < entities_limit; i++) {
        entities[i] = init_entity(world, '.');
    }


    char option;
    do {
        system("clear");
        update_world(world, entities, entities_limit);
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
    const char (*map)[world->width] = (char(*)[world->width]) world->raw_table;

    // Cálculos para ver de donde a donde se va a ver en situaciones normales (muy organizado y simple xd)
    Position the_quadrant_we_are_in = {
        .y = player->current_position.y / terminal_resolution.height,
        .x = player->current_position.x / terminal_resolution.width
    };

    Position quadrant_start_point = {
        .y = terminal_resolution.height * the_quadrant_we_are_in.y,
        .x = terminal_resolution.width * the_quadrant_we_are_in.x
    };

    Position quadrant_end_point = {
        .y = quadrant_start_point.y + terminal_resolution.height,
        .x = quadrant_start_point.x + terminal_resolution.width
    };

    if (quadrant_end_point.y > world->length) { quadrant_end_point.y = world->length; }
    if (quadrant_end_point.x > world->width)  { quadrant_end_point.x = world->width; }

    //Fin de los cálculos

    uint16_t row, column;
    for (row = quadrant_start_point.y; row < quadrant_end_point.y; row++) {
        for (column = quadrant_start_point.x; column < quadrant_end_point.x; column++) {
            printf("%c", map[row][column]);
        }
        printf("\n");
    }
    printf("P(%d, %d)\n", 
            player->current_position.y, 
            player->current_position.x
    );
}