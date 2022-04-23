#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include "world.h"

/*
 * Para obener correctamente el mapa desde la estructura world, necesitamos:
 * char (*world_map)[world_struct->width] = (char(*)[world_struct->width]) world_struct->world_ptr;
 * O similar.
 * Mas información en: https://stackoverflow.com/questions/54709981/how-to-initiliaze-a-dynamic-2d-array-inside-a-struct-in-c
 */

uint8_t determine_entity_stack_position(struct Entity *stack[]);
int32_t rand_min_max(int32_t min, int32_t max);

struct World *init_world(const uint16_t length, const uint16_t width) {
    struct World *world = calloc(1, sizeof (*world) + sizeof(struct TaggedCell[length][width])); // Se inicializa con 0s

    world->length = length;
    world->width = width;

    struct TaggedCell (*cell_map)[width] = (struct TaggedCell(*)[width]) world->cells;
    struct TaggedCell *cell;
    int row, column;
    for (row = 0; row < length; row++) {
        for (column = 0; column < width; column++) {
            cell = &cell_map[row][column];

            cell->tag = ENTITY_STACK;
        }
    }

    return world;
}

struct Entity *init_entity(const struct World *world, wchar_t character) {
    struct TaggedCell (*cell_map)[world->width] = (struct TaggedCell(*)[world->width]) world->cells;

    struct TaggedCell *cell;
    struct Position initial_position;
    do {
        initial_position = (struct Position) {
            .x = rand_min_max(0, world->width - 1),
            .y = rand_min_max(0, world->length - 1)
        };
        
        cell = &cell_map[initial_position.y][initial_position.x];
    } while (cell->tag == CHARACTER);

    // Las posición actual y anterior son la misma al principio
    struct Entity *entity = malloc(sizeof(struct Entity));
    entity->current_position = (struct Position) {
        initial_position.x,
        initial_position.y
    };
    entity->previous_position = (struct Position) {
        initial_position.x,
        initial_position.y
    };
    entity->position_change_request = (struct PositionChangeRequest) {
        .is_requesting = false,
        .delta_x = 0,
        .delta_y = 0
    };
    entity->character = character;
    entity->color = NO_COLOR;
    entity->stack_index = determine_entity_stack_position(cell->cell.entity_stack);

    cell->cell.entity_stack[entity->stack_index] = entity;

    return entity;
}

uint8_t progresive_position_change(struct Entity *entity, const struct World *world) {
    struct TaggedCell (*cell_map)[world->width] = (struct TaggedCell(*)[world->width]) world->cells;

    entity->previous_position = entity->current_position;

    struct PositionChangeRequest *const change_request = &entity->position_change_request;
    struct Position requested_closer_position = entity->current_position;

    if (change_request->delta_y > 0) {
        requested_closer_position.y++;
        change_request->delta_y--;
    }
    if (change_request->delta_y < 0) {
        requested_closer_position.y--;
        change_request->delta_y++;
    }

    if (change_request->delta_x > 0) {
        requested_closer_position.x++;
        change_request->delta_x--;
    }
    if (change_request->delta_x < 0) {
        requested_closer_position.x--;
        change_request->delta_x++;
    }

    if (change_request->delta_x == 0 || change_request->delta_y == 0) {
        entity->position_change_request.is_requesting = false;
    }

    bool is_out_of_bounds = (requested_closer_position.x >= world->width) || (requested_closer_position.y >= world->length);
    bool is_over_something = (cell_map[requested_closer_position.y][requested_closer_position.x].tag == CHARACTER);
    if (is_out_of_bounds || is_over_something) {
        return 1;
    }

    entity->current_position = requested_closer_position;

    // Quitamos la entidad de la celda en la que estaba
    struct TaggedCell *cell = &cell_map[entity->previous_position.y][entity->previous_position.x];
    cell->cell.entity_stack[entity->stack_index] = NULL;

    // Y la ponemos en la nueva celda
    cell = &cell_map[entity->current_position.y][entity->current_position.x];
    entity->stack_index = determine_entity_stack_position(cell->cell.entity_stack);
    cell->cell.entity_stack[entity->stack_index] = entity;

    return 0;
}

int32_t rand_min_max(int32_t min, int32_t max) {
    return (rand() % max) + min;
}

uint8_t determine_entity_stack_position(struct Entity *stack[]) {
    for (uint8_t i = 0; i < ENTITY_STACK_INSIDE_CELL_LIMIT; i++) {
        if (stack[i] == NULL) {
            return i;
        }
    }
    return 255;
}