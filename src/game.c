#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "game.h"
#include "pseudoconio.h"
#include "world.h"

const uint8_t target_y_fov = 12;
const uint8_t target_x_fov = 40;


typedef struct {
    uint8_t positive_axis;
    uint8_t negative_axis;
} OneDimensionVisibilityLimit;

typedef struct {
    OneDimensionVisibilityLimit x_limit;
    OneDimensionVisibilityLimit y_limit;
} VisibilityLimit;

void render_visible(const World *world, Entity *player);
bool player_move(uint16_t y, uint16_t x, Entity *player, World world);

void new_game() {
    World *world = init_world(100, 200);
    const char (*map)[world->width] = (char(*)[world->width]) world->world_ptr;
    int row, column;
    Entity *player = init_entity(world, '*');
    Entity *entities[1];
    entities[0] = player;
    char option;
    do {
        system("clear");
        update_world(world, entities, 1);
        render_visible(world, player);
        option = getch();
        bool exceeds_limits;
        switch (option) {
            case 'w':
                player->current_position->y -=1;
                break;
            case 'a':
                player->current_position->x -=1;
                break;
            case 's':
                player->current_position->y += 1;
                break;
            case 'd':
                player->current_position->x += 1;
                break;
        }
    } while(1);
}

void render_visible(const World *world, Entity *player) {
    const char (*map)[world->width] = (char(*)[world->width]) world->world_ptr;



    VisibilityLimit render_limit;
    render_limit = (VisibilityLimit) {
        .x_limit = (OneDimensionVisibilityLimit) {
            .positive_axis = (player->current_position->x + target_x_fov > world->width)? world->width: player->current_position->x + target_x_fov,
            .negative_axis = (player->current_position->x - target_x_fov < 0)? 0: player->current_position->x - target_x_fov,
        },
        .y_limit = (OneDimensionVisibilityLimit) {
            .positive_axis = (player->current_position->y + target_y_fov > world->height)? world->height: player->current_position->y + target_y_fov,
            .negative_axis = (player->current_position->y - target_y_fov < 0)? 0: player->current_position->y - target_y_fov,
        },
    };

    int row, column;
    for (row = render_limit.y_limit.negative_axis; row < render_limit.y_limit.positive_axis; row++) {
        for (column = render_limit.x_limit.negative_axis; column < render_limit.x_limit.positive_axis; column++) {
            printf("%c", map[row][column]);
        }
        printf("\n");
    }
}