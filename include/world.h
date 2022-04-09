#ifndef WORLD_H

#define WORLD_H

#include <stdint.h>

typedef struct {
    uint16_t height;
    uint16_t width;
    char world_ptr[]; //Se supone que esto es un puntero a un arreglo bidimensional
} World;
World *init_world(const uint16_t height, const uint16_t width);
typedef struct {
    uint16_t y_coordinate;
    uint16_t x_coordinate;
    char character;
} Entity;
Entity *init_entity(World *world);
bool update_world(World *world_struct, Entity *entities[], uint16_t array_length);
#endif