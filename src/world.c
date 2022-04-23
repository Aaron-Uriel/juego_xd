#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#include "world.h"

uint8_t determine_entity_stack_position(struct Entity *stack[]);
int32_t rand_min_max(int32_t min, int32_t max);

void init_world(struct TaggedCell world[WORLD_LENGTH][WORLD_WIDTH]) {
    struct TaggedCell *tagged_cell;
    uint16_t row, column, i;
    for (row = 0; row < WORLD_LENGTH; row++) {
        for (column = 0; column < WORLD_WIDTH; column++) {
            tagged_cell = &world[row][column];

            tagged_cell->tag = ENTITY_STACK;
            for (i = 0; i < ENTITY_STACK_INSIDE_CELL_LIMIT; i++) {
                tagged_cell->cell.entity_stack[i] = NULL;
            }
        }
    }
}

struct Entity *init_entity(struct TaggedCell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character) {

    struct TaggedCell *tagged_cell;
    struct Position initial_position;
    do {
        initial_position = (struct Position) {
            .x = rand_min_max(0, WORLD_WIDTH - 1),
            .y = rand_min_max(0, WORLD_LENGTH - 1)
        };
        
        tagged_cell = &world[initial_position.y][initial_position.x];
    } while (tagged_cell->tag == CHARACTER);

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
    entity->stack_index = determine_entity_stack_position(tagged_cell->cell.entity_stack);

    tagged_cell->cell.entity_stack[entity->stack_index] = entity;

    return entity;
}

uint8_t progresive_position_change(struct Entity *entity, struct TaggedCell world[WORLD_LENGTH][WORLD_WIDTH]) {
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

    bool is_out_of_bounds = (requested_closer_position.x >= WORLD_WIDTH) || (requested_closer_position.y >= WORLD_LENGTH);
    bool is_over_something = (world[requested_closer_position.y][requested_closer_position.x].tag == CHARACTER);
    if (is_out_of_bounds || is_over_something) {
        return 1;
    }

    entity->current_position = requested_closer_position;

    // Quitamos la entidad de la celda en la que estaba
    struct TaggedCell *tagged_cell = &world[entity->previous_position.y][entity->previous_position.x];
    tagged_cell->cell.entity_stack[entity->stack_index] = NULL;

    // Y la ponemos en la nueva celda
    tagged_cell = &world[entity->current_position.y][entity->current_position.x];
    entity->stack_index = determine_entity_stack_position(tagged_cell->cell.entity_stack);
    tagged_cell->cell.entity_stack[entity->stack_index] = entity;

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