#include "entities.h"
#include <string.h>

void player_init(Player *p, int id, const char *pseudo, int x, int y, int bomb_n) {
    p->id = id;
    strncpy(p->pseudo, pseudo, MAX_PSEUDO - 1);
    p->pseudo[MAX_PSEUDO - 1] = '\0';
    p->x = x;
    p->y = y;
    p->dir_x = 1;
    p->dir_y = 0;
    p->alive = 1;
    p->obstacles_detruits = 0;
    p->bombes_posees = 0;
    p->has_active_bomb = 0;
    p->bomb_range = bomb_n;
    p->killed_by = 0;
    p->move_cd = 0;
}

int player_compute_score(const Player *p) {
    if (p->bombes_posees == 0) return 0;
    return (p->obstacles_detruits * 50) / p->bombes_posees;
}
