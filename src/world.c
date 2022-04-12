#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include <assert.h>

#include "world.h"

/*
   Para obener correctamente el mapa desde la estructura world, necesitamos:
   char (*world_map)[world_struct->width] = (char(*)[world_struct->width]) world_struct->world_ptr;
   O similar.
   Mas información en: https://stackoverflow.com/questions/54709981/how-to-initiliaze-a-dynamic-2d-array-inside-a-struct-in-c
*/


void fill_world(const uint16_t height, const uint16_t width, char (*map)[*]);
int32_t rand_min_max(int32_t min, int32_t max);

Position *init_position() {
    Position *position_struct = malloc(sizeof(Position));
    return position_struct;
}

Position *init_and_set_position(uint16_t x, uint16_t y) {
    Position *position_struct = malloc(sizeof(Position));
    position_struct->x = x;
    position_struct->y = y;

    return position_struct;
}

void position_set(uint16_t x, uint16_t y, Position *position_struct) {
    position_struct->x = x;
    position_struct->y = y;
}

World *init_world(const uint16_t height, const uint16_t width) {
    World *world_struct = calloc(1, sizeof (*world_struct) + sizeof(char[height][width]));//Cambiar

    world_struct->height = height;
    world_struct->width = width;

    //Sintaxis del diablo, funciona como un arreglo bidimensional normal https://stackoverflow.com/questions/54709981/how-to-initiliaze-a-dynamic-2d-array-inside-a-struct-in-c
    char (*world)[width] = (char(*)[width]) world_struct->world_ptr;
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
    const Position initial_position = {
        .x = rand_min_max(1, world_struct->width - 2),
        .y = rand_min_max(1, world_struct->height - 2)
    };

    // Las posición actual y anterior son la misma al principio
    Entity *entity = malloc(sizeof(Entity));
    entity->current_position = init_and_set_position(
        initial_position.x,
        initial_position.y
    );
    entity->previous_position = init_and_set_position(
        initial_position.x,
        initial_position.y
    );
    entity->character = character;

    return entity;
}

bool update_world(World *world_struct, Entity *entities[], uint16_t entities_number) {
    char (*world_map)[world_struct->width] = (char(*)[world_struct->width]) world_struct->world_ptr;

    int entity_index;
    Position current_entity_position;
    for (entity_index = 0; entity_index < entities_number; entity_index++) {
        current_entity_position = *(entities[entity_index]->current_position);
        assert(current_entity_position.x == entities[entity_index]->current_position->x);

        world_map[current_entity_position.y][current_entity_position.x] = entities[entity_index]->character;
    }

    return 0;
}

int32_t rand_min_max(int32_t min, int32_t max) {
    srand(time(NULL));
    return (rand() % max) + min;
}