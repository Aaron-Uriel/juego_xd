#ifndef WORLD_H

#define WORLD_H

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

#include "colors.h"

struct Position {
    uint16_t x;
    uint16_t y;
};

struct Entity {
    struct Position current_position;
    struct Position previous_position;
    wchar_t character;
    enum Colors color;
};

enum CellTag { CHAR, ENTITIES_STACK };
union Cell {
    wchar_t character;
    struct Entity *stacked_entities; // Peligroso
};

struct TaggedCell {
    enum CellTag tag;
    union Cell cell;
};


struct World {
    uint16_t length;
    uint16_t width;
    wchar_t raw_world[]; // No tocar directamente
};

struct World *world_allocate(const uint16_t height, const uint16_t width);
struct World *init_world(const uint16_t height, const uint16_t width);
struct Entity *init_entity(const struct World *world, wchar_t character);

uint8_t request_change_of_position(const int8_t delta_x, const int8_t delta_y, struct Entity *entity, const struct World *world_struct);


#endif