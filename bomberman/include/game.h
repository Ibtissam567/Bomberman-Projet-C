#ifndef GAME_H
#define GAME_H

#include "arena.h"
#include "entities.h"
#include "config.h"
#include "input.h"

typedef enum {
    MODE_MONO = 1,
    MODE_MULTI = 2
} GameMode;

typedef struct {
    Arena arena;
    Player players[2];
    int nb_players;
    Bomb bombs[MAX_BOMBS];
    int explosion[MAX_HEIGHT][MAX_WIDTH];
    int running;
    int game_over;
    int paused;
    int bomb_n;
    char message[160];
    int evt_bomb;
    int evt_boom;
    int evt_pickup;
    int evt_kill;
    int evt_over;
    int shake;
} Game;

int  game_init(Game *g, GameMode mode, const char *arena_file, int nb_obstacles,
               int bomb_n, const char *pseudo1, const char *pseudo2);
void game_apply_input(Game *g, const InputState *in);
void game_update(Game *g);
void game_save_scores(Game *g);
void game_clear_events(Game *g);
void game_run(Game *g);

#endif /* GAME_H */
