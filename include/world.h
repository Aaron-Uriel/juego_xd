#ifndef WORLD_H

#define WORLD_H

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

#include "colors.h"
#include "resolution.h"

/*
 * Explicación de la lógica del mundo.
 * El mundo es un arreglo de dos dimensiones localizado en la memoria dinámica
 * Cada elemento del mundo es una celda, y esta celda puede ser un carácter (por ejemplo un bloque) o un
 * arreglo de entidades, o sea, un indicador de las entidades que están sobre dicha celda.
 * Las celdas que son arreglos de punteros de entidades, son las partes donde podrán andar las entidades.
 */

enum Constants {
    WORLD_LENGTH = 100,
    WORLD_WIDTH = 200,
    ENTITY_LIMIT = 128,
    STACK_LIMIT = 20,
    UNINITIALIZED_8 = UINT8_MAX,
    UNINITIALIZED_16 = UINT16_MAX,
    UNINITIALIZED_32 = UINT32_MAX
};

struct Position {
    uint16_t x;
    uint16_t y;
};

struct Entity {
    // Cosas técnicas
    struct Position current_position;
    struct Position previous_position;

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

enum RequestKind { POSITION_REQUEST, ATTACK_REQUEST };
struct EntityRequest {
    struct Entity *requesting_entity;
    enum RequestKind kind;
    union InternalRequest request;
};

union InternalCell {
    wchar_t character;
    struct Entity *entity;
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

void init_world(struct Cell world[WORLD_LENGTH][WORLD_WIDTH]);
void init_visible_world(struct VisibleWorld * const, struct Cell world[WORLD_LENGTH][WORLD_WIDTH]);
void init_entity(struct Entity *, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character);
void init_player(struct Entity * const new_player, struct Cell world[WORLD_LENGTH][WORLD_WIDTH], wchar_t character, struct Resolution gameplay_resolution);

bool try_to_update_entity_position(struct Entity * const , struct PositionChangeRequest , struct VisibleWorld * const );


#endif