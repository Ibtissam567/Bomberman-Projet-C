#ifndef ENTITIES_H
#define ENTITIES_H

#include "config.h"

typedef struct {
    int id;                 /* 1 ou 2 */
    char pseudo[MAX_PSEUDO];
    int x, y;
    int dir_x, dir_y;       /* derniere direction (bombe posee DEVANT le joueur) */
    int alive;
    int obstacles_detruits;
    int bombes_posees;
    int has_active_bomb;
    int bomb_range;         /* portee personnelle (bonus possible) */
    int killed_by;          /* 0 = vivant / suicide, sinon id du proprietaire de la bombe */
    int move_cd;            /* anti-teleport : delai entre deux pas */
} Player;

typedef struct {
    int active;
    int x, y;
    int owner_id;
    int range;
    int ticks_restants;
} Bomb;

#define MAX_BOMBS 4

void player_init(Player *p, int id, const char *pseudo, int x, int y, int bomb_n);
int player_compute_score(const Player *p);

#endif /* ENTITIES_H */
