#ifndef CONFIG_H
#define CONFIG_H

/* ===================== Paramètres généraux du jeu ===================== */

#define MAX_WIDTH   40
#define MAX_HEIGHT  20

/* n : la bombe explose après n secondes et a une portée de n
 * (consigne du sujet). Modifiable en jeu via le menu. */
#define DEFAULT_BOMB_N       3

#define TICK_MS              50     /* boucle fluide */
#define MOVE_COOLDOWN_TICKS  3      /* un pas toutes les ~150 ms */
#define DEFAULT_NB_OBSTACLES 20

#define MAX_PSEUDO   32
#define SCORES_FILE  "scores.txt"
#define MAX_SCORES_KEPT 10

#define EXPLOSION_TICKS      4      /* duree d'affichage de l'explosion */
#define POWERUP_CHANCE       18     /* % de chance qu'un obstacle laisse un bonus */

#endif /* CONFIG_H */
