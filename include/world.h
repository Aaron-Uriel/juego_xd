#ifndef WORLD_H

#define WORLD_H

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

#include "colors.h"
#include "resolution.h"
#include "utils.h"
#include "game.h"

/*
 * Explicación de la lógica del mundo.
 * El mundo es un arreglo de dos dimensiones localizado en la memoria dinámica
 * Cada elemento del mundo es una celda, y esta celda puede ser un carácter (por ejemplo un bloque) o un
 * arreglo de entidades, o sea, un indicador de las entidades que están sobre dicha celda.
 * Las celdas que son arreglos de punteros de entidades, son las partes donde podrán andar las entidades.
 */

struct Entity {
    // Cosas técnicas
    struct Position current_position;
    struct Position previous_position;
    enum { FACING_NORTH, FACING_EAST, FACING_SOUTH, FACING_WEST } facing;

    // Atributos
    wchar_t character;
    enum Colors color;
};

struct PositionChangeRequest {
    enum { AXIS_Y, AXIS_X } axis;
    enum { DELTA_POSITIVE, DELTA_NEGATIVE } delta;
};

union InternalRequest {
    struct PositionChangeRequest position_change_request;
};

struct EntityRequest {
    struct Entity *requesting_entity;
    enum { ENTITY_REQUEST_KIND_POSITION, ENTITY_REQUEST_KIND_ATTACK } kind;
    union {
        struct PositionChangeRequest position_change_request;
    } content;
};

struct Cell {
    enum { KIND_CHARACTER, KIND_ENTITY_HOLDER } kind;
    union {
        wchar_t character;
        struct Entity *entity_holder;
    } content;
};

struct VisibleWorld {
    struct Cell (*world)[WORLD_WIDTH];
    struct Position quadrant;
    bool is_new_quadrant;
    struct Position start_point;
    struct Position end_point;
    struct Entity *visible_entities[STACK_LIMIT];
    struct EntityRequest requests_stack[STACK_LIMIT];
};

void world_init(struct Cell world[WORLD_LENGTH][WORLD_WIDTH]);
void visible_world_init(struct VisibleWorld * const, struct Cell world[WORLD_LENGTH][WORLD_WIDTH]);
void visible_world_update(struct VisibleWorld * const visible_world, struct Entity * const entities[STACK_LIMIT], const struct Resolution resolution);

void entity_init(struct Entity * const new_entity, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], const wchar_t character);
void entity_player_init(struct Entity * const new_player, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], const wchar_t character, const struct Resolution gameplay_resolution);
bool entity_request_add(const struct EntityRequest request, struct EntityRequest requests_stack[STACK_LIMIT]);
bool entity_try_to_update_position(struct Entity * const entity, const struct PositionChangeRequest position_request, const struct VisibleWorld * const visible_world);
#endif