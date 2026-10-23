#ifndef ARENA_H
#define ARENA_H

#include "config.h"

typedef enum {
    CELL_EMPTY = 0,
    CELL_WALL,
    CELL_OBSTACLE,
    CELL_POWER_RANGE
} CellType;

typedef struct {
    CellType grid[MAX_HEIGHT][MAX_WIDTH];
    int width;
    int height;
    int obstacles_restants;
} Arena;

int arena_load(Arena *arena, const char *filename);
void arena_place_obstacles(Arena *arena, int nb_obstacles);
void arena_random_free_cell(const Arena *arena, int *x, int *y,
                             const int occupied[][2], int nb_occupied);

int arena_in_bounds(const Arena *arena, int x, int y);
int arena_is_wall(const Arena *arena, int x, int y);
int arena_is_obstacle(const Arena *arena, int x, int y);
int arena_is_free(const Arena *arena, int x, int y);
int arena_destroy_obstacle(Arena *arena, int x, int y);

#endif /* ARENA_H */
