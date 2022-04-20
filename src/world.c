#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include "world.h"

/*
   Para obener correctamente el mapa desde la estructura world, necesitamos:
   char (*world_map)[world_struct->width] = (char(*)[world_struct->width]) world_struct->world_ptr;
   O similar.
   Mas información en: https://stackoverflow.com/questions/54709981/how-to-initiliaze-a-dynamic-2d-array-inside-a-struct-in-c
*/

int32_t rand_min_max(int32_t min, int32_t max);

World *init_world(const uint16_t height, const uint16_t width) {
    World *world_struct = calloc(1, sizeof (*world_struct) + sizeof(wchar_t[height][width]));//Cambiar

    world_struct->length = height;
    world_struct->width = width;

    wchar_t (*world_map)[width] = (wchar_t(*)[width]) world_struct->raw_table;
    int row, column;
    for (row = 0; row < height; row++) {
        for (column = 0; column < width; column++) {
            world_map[row][column] = ' ';
        }
    }

    return world_struct;
}

Entity *init_entity(const World *world_struct, wchar_t character) {
    wchar_t (*world_map)[world_struct->width] = (wchar_t(*)[world_struct->width]) world_struct->raw_table;

    Position initial_position;
    do {
        initial_position = (Position) {
            .x = rand_min_max(0, world_struct->width - 1),
            .y = rand_min_max(0, world_struct->length - 1)
        };
    } while (world_map[initial_position.y][initial_position.x] != ' ');

    // Las posición actual y anterior son la misma al principio
    Entity *entity = malloc(sizeof(Entity));
    entity->current_position = (Position) {
        initial_position.x,
        initial_position.y
    };
    entity->previous_position = (Position) {
        initial_position.x,
        initial_position.y
    };
    entity->character = character;
    entity->color = NO_COLOR;

    return entity;
}

uint8_t request_change_of_position(const int8_t delta_x, const int8_t delta_y, Entity *entity, const World *world) {
    wchar_t (*world_map)[world->width] = (wchar_t(*)[world->width]) world->raw_table;

    entity->previous_position = entity->current_position;

    const Position requested_new_position = {
        .x = entity->current_position.x + delta_x,
        .y = entity->current_position.y + delta_y
    };

    bool is_out_of_bounds = (requested_new_position.x >= world->width) || (requested_new_position.y >= world->length);
    bool is_over_something = (world_map[requested_new_position.y][requested_new_position.x] != ' ');
    if (is_out_of_bounds || is_over_something) {
        return 1;
    }

    entity->current_position = requested_new_position;

    return 0;
}

int32_t rand_min_max(int32_t min, int32_t max) {
    return (rand() % max) + min;
}