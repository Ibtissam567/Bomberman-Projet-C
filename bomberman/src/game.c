#include "game.h"
#include "input.h"
#include "score.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int bomb_edge[2];
static int pause_edge;

static int cell_blocked_by_bomb(const Game *g, int x, int y) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (g->bombs[i].active && g->bombs[i].x == x && g->bombs[i].y == y) return 1;
    }
    return 0;
}

int game_init(Game *g, GameMode mode, const char *arena_file, int nb_obstacles,
              int bomb_n, const char *pseudo1, const char *pseudo2) {
    memset(g, 0, sizeof(*g));
    g->bomb_n = (bomb_n < 1) ? 1 : bomb_n;

    if (arena_load(&g->arena, arena_file) != 0) {
        fprintf(stderr, "Impossible de charger l'arene.\n");
        return -1;
    }

    arena_place_obstacles(&g->arena, nb_obstacles);
    g->nb_players = (mode == MODE_MULTI) ? 2 : 1;

    int occupied[2][2];
    int nb_occ = 0;
    int x1, y1;
    arena_random_free_cell(&g->arena, &x1, &y1, occupied, nb_occ);
    player_init(&g->players[0], 1, pseudo1, x1, y1, g->bomb_n);
    occupied[nb_occ][0] = x1;
    occupied[nb_occ][1] = y1;
    nb_occ++;

    if (g->nb_players == 2) {
        int x2, y2;
        arena_random_free_cell(&g->arena, &x2, &y2, occupied, nb_occ);
        player_init(&g->players[1], 2, pseudo2, x2, y2, g->bomb_n);
    }

    g->running = 1;
    g->game_over = 0;
    g->paused = 0;
    g->message[0] = '\0';
    bomb_edge[0] = bomb_edge[1] = 0;
    pause_edge = 0;
    return 0;
}

void game_clear_events(Game *g) {
    g->evt_bomb = g->evt_boom = g->evt_pickup = g->evt_kill = g->evt_over = 0;
}

void game_save_scores(Game *g) {
    for (int i = 0; i < g->nb_players; i++) {
        save_score(g->players[i].pseudo, player_compute_score(&g->players[i]));
    }
}

static char cell_char_at(Game *g, int x, int y, int *color) {
    if (g->explosion[y][x] > 0) {
        *color = COL_BOOM;
        return '*';
    }
    for (int i = 0; i < g->nb_players; i++) {
        if (g->players[i].alive && g->players[i].x == x && g->players[i].y == y) {
            *color = (i == 0) ? COL_P1 : COL_P2;
            return 'B';
        }
    }
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (g->bombs[i].active && g->bombs[i].x == x && g->bombs[i].y == y) {
            *color = COL_BOMB;
            return 'o';
        }
    }
    CellType c = g->arena.grid[y][x];
    if (c == CELL_WALL) { *color = COL_WALL; return '#'; }
    if (c == CELL_OBSTACLE) { *color = COL_OBS; return 'X'; }
    if (c == CELL_POWER_RANGE) { *color = COL_POWER; return '+'; }
    *color = COL_TEXT;
    return ' ';
}

static void render(Game *g) {
    console_home();
    console_set_color(COL_TITLE);
    printf("=== BOMBERMAN  |  n=%d (timer + portee) ===          \n\n", g->bomb_n);
    console_reset_color();

    for (int y = 0; y < g->arena.height; y++) {
        for (int x = 0; x < g->arena.width; x++) {
            int col;
            char ch = cell_char_at(g, x, y, &col);
            console_set_color(col);
            putchar(ch);
        }
        console_reset_color();
        printf("   \n");
    }

    printf("\n");
    for (int i = 0; i < g->nb_players; i++) {
        Player *p = &g->players[i];
        console_set_color((i == 0) ? COL_P1 : COL_P2);
        printf("J%d %-12s %s  X:%2d  bombes:%2d  portee:%d  score:%3d\n",
               p->id, p->pseudo, p->alive ? "  " : "KO",
               p->obstacles_detruits, p->bombes_posees, p->bomb_range,
               player_compute_score(p));
        console_reset_color();
    }
    printf("Obstacles restants : %d     score = (X detruits * 50) / bombes\n",
           g->arena.obstacles_restants);
    printf("J1: ZQSD + W   J2: fleches + ESPACE   P: pause   ESC: quitter\n");
    printf("Bonus + : augmente la portee de vos prochaines bombes.\n");

    if (g->paused) {
        console_set_color(COL_TITLE);
        printf("\n*** PAUSE ***  (relacher P puis rappuyer pour reprendre)          \n");
        console_reset_color();
    }
    if (g->message[0] != '\0') {
        console_set_color(COL_BOMB);
        printf("\n>>> %s\n", g->message);
        console_reset_color();
    }
}

static Bomb *find_free_bomb_slot(Game *g) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (!g->bombs[i].active) return &g->bombs[i];
    }
    return NULL;
}

static void pickup_if_any(Game *g, Player *p) {
    if (g->arena.grid[p->y][p->x] == CELL_POWER_RANGE) {
        g->arena.grid[p->y][p->x] = CELL_EMPTY;
        if (p->bomb_range < 8) p->bomb_range++;
        g->evt_pickup = 1;
    }
}

static void try_move(Game *g, Player *p, int dx, int dy) {
    if (!p->alive) return;
    p->dir_x = dx;
    p->dir_y = dy;
    if (p->move_cd > 0) return;
    int nx = p->x + dx;
    int ny = p->y + dy;

    if (!arena_in_bounds(&g->arena, nx, ny)) return;
    if (arena_is_wall(&g->arena, nx, ny)) return;
    if (arena_is_obstacle(&g->arena, nx, ny)) return;
    if (cell_blocked_by_bomb(g, nx, ny)) return;

    for (int i = 0; i < g->nb_players; i++) {
        Player *other = &g->players[i];
        if (other != p && other->alive && other->x == nx && other->y == ny) return;
    }

    p->x = nx;
    p->y = ny;
    p->move_cd = MOVE_COOLDOWN_TICKS;
    pickup_if_any(g, p);
}

static void try_place_bomb(Game *g, Player *p) {
    if (!p->alive) return;
    if (p->has_active_bomb) return;

    int bx = p->x + p->dir_x;
    int by = p->y + p->dir_y;

    if (!arena_in_bounds(&g->arena, bx, by)) return;
    if (arena_is_wall(&g->arena, bx, by)) return;
    if (arena_is_obstacle(&g->arena, bx, by)) return;
    if (cell_blocked_by_bomb(g, bx, by)) return;

    for (int i = 0; i < g->nb_players; i++) {
        if (g->players[i].alive && g->players[i].x == bx && g->players[i].y == by) return;
    }

    Bomb *slot = find_free_bomb_slot(g);
    if (!slot) return;

    slot->active = 1;
    slot->x = bx;
    slot->y = by;
    slot->owner_id = p->id;
    slot->range = p->bomb_range;
    slot->ticks_restants = (g->bomb_n * 1000) / TICK_MS;
    if (slot->ticks_restants < 1) slot->ticks_restants = 1;

    p->has_active_bomb = 1;
    p->bombes_posees++;
    g->evt_bomb = 1;
}

static void mark_explosion(Game *g, int x, int y) {
    if (arena_in_bounds(&g->arena, x, y)) {
        g->explosion[y][x] = EXPLOSION_TICKS;
    }
}

static void hit_player_at(Game *g, int x, int y, int owner_id) {
    for (int i = 0; i < g->nb_players; i++) {
        Player *p = &g->players[i];
        if (p->alive && p->x == x && p->y == y) {
            p->alive = 0;
            p->killed_by = owner_id;
            g->evt_kill = 1;
        }
    }
}

static void explode_bomb(Game *g, Bomb *b) {
    static const int dirs[4][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

    mark_explosion(g, b->x, b->y);
    hit_player_at(g, b->x, b->y, b->owner_id);

    for (int d = 0; d < 4; d++) {
        for (int step = 1; step <= b->range; step++) {
            int cx = b->x + dirs[d][0] * step;
            int cy = b->y + dirs[d][1] * step;

            if (!arena_in_bounds(&g->arena, cx, cy)) break;
            if (arena_is_wall(&g->arena, cx, cy)) break;

            mark_explosion(g, cx, cy);
            hit_player_at(g, cx, cy, b->owner_id);

            int destroyed = arena_destroy_obstacle(&g->arena, cx, cy);
            if (destroyed) {
                for (int i = 0; i < g->nb_players; i++) {
                    if (g->players[i].id == b->owner_id) {
                        g->players[i].obstacles_detruits++;
                    }
                }
                break;
            }
        }
    }

    for (int i = 0; i < g->nb_players; i++) {
        if (g->players[i].id == b->owner_id) g->players[i].has_active_bomb = 0;
    }
    b->active = 0;
    g->evt_boom = 1;
    g->shake = 12;
}

static void update_bombs(Game *g) {
    for (int y = 0; y < g->arena.height; y++) {
        for (int x = 0; x < g->arena.width; x++) {
            if (g->explosion[y][x] > 0) g->explosion[y][x]--;
        }
    }
    for (int i = 0; i < MAX_BOMBS; i++) {
        Bomb *b = &g->bombs[i];
        if (!b->active) continue;
        b->ticks_restants--;
        if (b->ticks_restants <= 0) explode_bomb(g, b);
    }
}

static void apply_player_input(Game *g, int idx, int up, int down, int left, int right, int bomb) {
    Player *p = &g->players[idx];
    if (up && !down) try_move(g, p, 0, -1);
    else if (down && !up) try_move(g, p, 0, 1);
    if (left && !right) try_move(g, p, -1, 0);
    else if (right && !left) try_move(g, p, 1, 0);

    if (bomb && !bomb_edge[idx]) try_place_bomb(g, p);
    bomb_edge[idx] = bomb;
}

void game_apply_input(Game *g, const InputState *in) {
    if (in->quit) {
        g->running = 0;
        snprintf(g->message, sizeof(g->message), "Partie interrompue.");
        return;
    }

    if (in->pause && !pause_edge) g->paused = !g->paused;
    pause_edge = in->pause;
    if (g->paused) return;

    apply_player_input(g, 0, in->p1_up, in->p1_down, in->p1_left, in->p1_right, in->p1_bomb);
    if (g->nb_players == 2) {
        apply_player_input(g, 1, in->p2_up, in->p2_down, in->p2_left, in->p2_right, in->p2_bomb);
    }
}

static void declare_score_winner(Game *g) {
    if (g->nb_players == 1) {
        snprintf(g->message, sizeof(g->message),
                 "Tous les obstacles sont detruits ! Score de %s : %d",
                 g->players[0].pseudo, player_compute_score(&g->players[0]));
        return;
    }
    int s1 = player_compute_score(&g->players[0]);
    int s2 = player_compute_score(&g->players[1]);
    if (s1 > s2) {
        snprintf(g->message, sizeof(g->message),
                 "Tous les obstacles detruits ! %s gagne (score %d > %d).",
                 g->players[0].pseudo, s1, s2);
    } else if (s2 > s1) {
        snprintf(g->message, sizeof(g->message),
                 "Tous les obstacles detruits ! %s gagne (score %d > %d).",
                 g->players[1].pseudo, s2, s1);
    } else {
        snprintf(g->message, sizeof(g->message),
                 "Tous les obstacles detruits ! Egalite (%d pts).", s1);
    }
}

static int check_end_condition(Game *g) {
    if (g->arena.obstacles_restants <= 0) {
        declare_score_winner(g);
        return 1;
    }

    if (g->nb_players == 2) {
        int a1 = g->players[0].alive;
        int a2 = g->players[1].alive;
        if (a1 && a2) return 0;

        if (!a1 && a2) {
            if (g->players[0].killed_by == 2) {
                snprintf(g->message, sizeof(g->message),
                         "%s a ete pris par une bombe de %s ! %s gagne.",
                         g->players[0].pseudo, g->players[1].pseudo, g->players[1].pseudo);
            } else {
                snprintf(g->message, sizeof(g->message),
                         "%s a explose sur sa propre bombe. %s gagne.",
                         g->players[0].pseudo, g->players[1].pseudo);
            }
            return 1;
        }
        if (a1 && !a2) {
            if (g->players[1].killed_by == 1) {
                snprintf(g->message, sizeof(g->message),
                         "%s a ete pris par une bombe de %s ! %s gagne.",
                         g->players[1].pseudo, g->players[0].pseudo, g->players[0].pseudo);
            } else {
                snprintf(g->message, sizeof(g->message),
                         "%s a explose sur sa propre bombe. %s gagne.",
                         g->players[1].pseudo, g->players[0].pseudo);
            }
            return 1;
        }
        snprintf(g->message, sizeof(g->message), "Les deux joueurs ont explose ! Match nul.");
        return 1;
    }

    if (!g->players[0].alive) {
        snprintf(g->message, sizeof(g->message),
                 "%s a explose ! Partie terminee. Score : %d",
                 g->players[0].pseudo, player_compute_score(&g->players[0]));
        return 1;
    }
    return 0;
}

void game_update(Game *g) {
    if (g->shake > 0) g->shake--;
    for (int i = 0; i < g->nb_players; i++) {
        if (g->players[i].move_cd > 0) g->players[i].move_cd--;
    }
    if (g->paused || g->game_over) return;
    update_bombs(g);
    if (check_end_condition(g)) {
        g->game_over = 1;
        g->evt_over = 1;
    }
}

void game_run(Game *g) {
    input_init();
    input_flush();

#ifdef _WIN32
    system("cls");
#else
    printf("\033[H\033[2J");
#endif

    while (g->running) {
        render(g);
        InputState in;
        input_poll(&in);
        game_clear_events(g);
        game_apply_input(g, &in);
        if (!g->running) break;

        game_update(g);
        if (g->evt_bomb) play_sfx(1);
        if (g->evt_boom) play_sfx(2);
        if (g->evt_pickup) play_sfx(3);
        if (g->evt_kill) play_sfx(4);
        if (g->evt_over) {
            play_sfx(5);
            render(g);
            break;
        }
        sleep_ms(TICK_MS);
    }

    input_restore();
    game_save_scores(g);

    printf("\n");
    display_best_scores();
    printf("\nAppuyez sur Entree pour revenir au menu...");
    fflush(stdout);
    getchar();
}
