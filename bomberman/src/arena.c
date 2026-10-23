#include "arena.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static FILE *open_arena_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (f) return f;
    f = fopen("..\\..\\arene.txt", "r");
    if (f) return f;
    f = fopen("..\\arene.txt", "r");
    return f;
}

int arena_load(Arena *arena, const char *filename) {
    FILE *f = open_arena_file(filename);
    if (!f) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier d'arene '%s'\n", filename);
        return -1;
    }

    memset(arena, 0, sizeof(*arena));
    for (int y = 0; y < MAX_HEIGHT; y++) {
        for (int x = 0; x < MAX_WIDTH; x++) {
            arena->grid[y][x] = CELL_WALL;
        }
    }

    char line[MAX_WIDTH + 16];
    int row = 0;
    int max_len = 0;

    while (row < MAX_HEIGHT && fgets(line, sizeof(line), f)) {
        int len = (int)strcspn(line, "\r\n");
        if (len == 0) continue;

        for (int col = 0; col < len && col < MAX_WIDTH; col++) {
            arena->grid[row][col] = (line[col] == '#') ? CELL_WALL : CELL_EMPTY;
        }
        if (len > max_len) max_len = len;
        row++;
    }
    fclose(f);

    if (row == 0) {
        fprintf(stderr, "Erreur : fichier d'arene vide ou invalide\n");
        return -1;
    }

    arena->height = row;
    arena->width = max_len;
    arena->obstacles_restants = 0;
    return 0;
}

int arena_in_bounds(const Arena *arena, int x, int y) {
    return x >= 0 && x < arena->width && y >= 0 && y < arena->height;
}

int arena_is_wall(const Arena *arena, int x, int y) {
    if (!arena_in_bounds(arena, x, y)) return 1;
    return arena->grid[y][x] == CELL_WALL;
}

int arena_is_obstacle(const Arena *arena, int x, int y) {
    if (!arena_in_bounds(arena, x, y)) return 0;
    return arena->grid[y][x] == CELL_OBSTACLE;
}

int arena_is_free(const Arena *arena, int x, int y) {
    if (!arena_in_bounds(arena, x, y)) return 0;
    return arena->grid[y][x] == CELL_EMPTY || arena->grid[y][x] == CELL_POWER_RANGE;
}

void arena_random_free_cell(const Arena *arena, int *x, int *y,
                             const int occupied[][2], int nb_occupied) {
    int try_x, try_y, ok;
    int guard = 0;
    do {
        try_x = rand() % arena->width;
        try_y = rand() % arena->height;
        ok = (arena->grid[try_y][try_x] == CELL_EMPTY);
        if (ok) {
            for (int i = 0; i < nb_occupied; i++) {
                if (occupied[i][0] == try_x && occupied[i][1] == try_y) {
                    ok = 0;
                    break;
                }
            }
        }
        guard++;
        if (guard > arena->width * arena->height * 50) {
            *x = 1;
            *y = 1;
            return;
        }
    } while (!ok);
    *x = try_x;
    *y = try_y;
}

void arena_place_obstacles(Arena *arena, int nb_obstacles) {
    int placed = 0;
    int guard = 0;
    int max_attempts = arena->width * arena->height * 10 + 100;

    while (placed < nb_obstacles && guard < max_attempts) {
        int x = rand() % arena->width;
        int y = rand() % arena->height;
        if (arena->grid[y][x] == CELL_EMPTY) {
            arena->grid[y][x] = CELL_OBSTACLE;
            placed++;
        }
        guard++;
    }
    arena->obstacles_restants = placed;
}

int arena_destroy_obstacle(Arena *arena, int x, int y) {
    if (!arena_in_bounds(arena, x, y)) return 0;
    if (arena->grid[y][x] == CELL_OBSTACLE) {
        if ((rand() % 100) < POWERUP_CHANCE) {
            arena->grid[y][x] = CELL_POWER_RANGE;
        } else {
            arena->grid[y][x] = CELL_EMPTY;
        }
        if (arena->obstacles_restants > 0) arena->obstacles_restants--;
        return 1;
    }
    return 0;
}
