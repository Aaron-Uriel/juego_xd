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


void fill_world(const uint16_t height, const uint16_t width, char (*map)[*]);
int32_t rand_min_max(int32_t min, int32_t max);

World *init_world(const uint16_t height, const uint16_t width) {
    World *world_struct = calloc(1, sizeof (*world_struct) + sizeof(char[height][width]));//Cambiar

    world_struct->length = height;
    world_struct->width = width;

    char (*world)[width] = (char(*)[width]) world_struct->raw_table;
    fill_world(height, width, *world);

    return world_struct;
}

void fill_world(const uint16_t height, const uint16_t width, char map[height][width]) {
    int row, column;
    for(row = 0; row < height; row++) {
        map[row][0] = '#';
        map[row][width - 1] = '#';
    }
    for(column = 0; column < width; column++) {
        map[0][column] = '#';
        map[height - 1][column] = '#';
    }
    for (row = 1; row < height - 1; row++) {
        for (column = 1; column < width - 1; column++) {
            map[row][column] = ' ';
        }
    }
}

Entity *init_entity(const World *world_struct, char character) {
    char (*world_map)[world_struct->width] = (char(*)[world_struct->width]) world_struct->raw_table;

    Position initial_position;
    do {
        initial_position = (Position) {
            .x = rand_min_max(1, world_struct->width - 2),
            .y = rand_min_max(1, world_struct->length - 2)
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

    return entity;
}

uint8_t request_change_of_position(const int8_t delta_x, const int8_t delta_y, Entity *entity, const World *world) {
    char (*world_map)[world->width] = (char(*)[world->width]) world->raw_table;

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

bool update_world(World *world_struct, Entity *entities[], uint16_t entities_number) {
    char (*world_map)[world_struct->width] = (char(*)[world_struct->width]) world_struct->raw_table;

    int entity_index;
    Position entity_current_position, entity_previous_position;
    bool has_entity_moved;
    for (entity_index = 0; entity_index < entities_number; entity_index++) {
        entity_current_position = entities[entity_index]->current_position;
        entity_previous_position = entities[entity_index]->previous_position;

        world_map[entity_current_position.y][entity_current_position.x] = entities[entity_index]->character;

        has_entity_moved = (entity_current_position.x != entity_previous_position.x) || (entity_current_position.y != entity_previous_position.y);
        if (has_entity_moved) {
            world_map[entity_previous_position.y][entity_previous_position.x] = ' ';
        }
    }

    return 0;
}

int32_t rand_min_max(int32_t min, int32_t max) {
    return (rand() % max) + min;
}