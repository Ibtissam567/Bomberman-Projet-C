#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game.h"
#include "score.h"
#include "input.h"

#ifdef _WIN32
#include <windows.h>
#endif

static void read_line(char *out, int max_len) {
    if (!fgets(out, max_len, stdin)) {
        out[0] = '\0';
        return;
    }
    out[strcspn(out, "\r\n")] = '\0';
}

static void read_pseudo(const char *prompt, char *out, int max_len) {
    printf("%s", prompt);
    fflush(stdout);
    read_line(out, max_len);
    if (out[0] == '\0') strncpy(out, "Joueur", (size_t)max_len - 1);
}

static int read_int(const char *prompt, int def_val, int min_v, int max_v) {
    char buf[32];
    printf("%s [%d] : ", prompt, def_val);
    fflush(stdout);
    read_line(buf, sizeof(buf));
    if (buf[0] == '\0') return def_val;
    int v = atoi(buf);
    if (v < min_v) v = min_v;
    if (v > max_v) v = max_v;
    return v;
}

static void print_banner(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[H\033[2J");
#endif
    printf("====================================================\n");
    printf("          BOMBERMAN  -  Projet Langage C\n");
    printf("====================================================\n");
    printf("  Murs #   Obstacles X   Joueurs B   Bombes o\n");
    printf("  score = (obstacles detruits * 50) / bombes posees\n");
    printf("====================================================\n\n");
}

static void play_match(GameMode mode, const char *arena_file, int nb_obstacles, int bomb_n) {
    char pseudo1[MAX_PSEUDO] = "Joueur1";
    char pseudo2[MAX_PSEUDO] = "Joueur2";

    read_pseudo("Pseudo du joueur 1 : ", pseudo1, MAX_PSEUDO);
    if (mode == MODE_MULTI) {
        read_pseudo("Pseudo du joueur 2 : ", pseudo2, MAX_PSEUDO);
    }

    printf("\nControles :\n");
    printf("  Joueur 1 : Z (haut)  Q (gauche)  S (bas)  D (droite)  W (bombe)\n");
    if (mode == MODE_MULTI) {
        printf("  Joueur 2 : fleches                                  ESPACE (bombe)\n");
    }
    printf("  Les deux joueurs bougent en meme temps (touches maintenues).\n");
    printf("  La bombe se pose sur la case DEVANT le joueur (derniere direction).\n");
    printf("  P = pause    ESC = quitter\n");
    printf("\nAppuyez sur Entree pour commencer...");
    fflush(stdout);
    getchar();

    Game g;
    if (game_init(&g, mode, arena_file, nb_obstacles, bomb_n, pseudo1, pseudo2) != 0) {
        printf("Impossible de demarrer (arene.txt introuvable ?).\n");
        printf("Entree pour revenir au menu...");
        getchar();
        return;
    }
    game_run(&g);
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleTitleA("Bomberman - Projet C");
    console_enable_colors();
#endif
    srand((unsigned int)time(NULL));

    const char *arena_file = "arene.txt";
    int nb_obstacles = DEFAULT_NB_OBSTACLES;
    int bomb_n = DEFAULT_BOMB_N;

    if (argc >= 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        printf("Usage : %s [mono|multi] [fichier_arene] [nb_obstacles] [n]\n", argv[0]);
        return EXIT_SUCCESS;
    }

    if (argc >= 2) {
        GameMode mode = MODE_MONO;
        if (strcmp(argv[1], "multi") == 0) mode = MODE_MULTI;
        if (argc >= 3) arena_file = argv[2];
        if (argc >= 4) nb_obstacles = atoi(argv[3]);
        if (argc >= 5) bomb_n = atoi(argv[4]);
        play_match(mode, arena_file, nb_obstacles, bomb_n);
        return EXIT_SUCCESS;
    }

    for (;;) {
        print_banner();
        printf("  1. Mono-bomberman  (1 joueur)\n");
        printf("  2. Multi-bomberman (2 joueurs, simultane)\n");
        printf("  3. Parametres (obstacles, n = timer/portee)\n");
        printf("  4. Meilleurs scores\n");
        printf("  5. Quitter\n\n");
        printf("Votre choix : ");
        fflush(stdout);

        char buf[16];
        read_line(buf, sizeof(buf));
        int choice = atoi(buf);

        if (choice == 1) {
            play_match(MODE_MONO, arena_file, nb_obstacles, bomb_n);
        } else if (choice == 2) {
            play_match(MODE_MULTI, arena_file, nb_obstacles, bomb_n);
        } else if (choice == 3) {
            printf("\nParametres actuels : obstacles=%d  n=%d\n", nb_obstacles, bomb_n);
            nb_obstacles = read_int("Nombre d'obstacles", nb_obstacles, 1, 80);
            bomb_n = read_int("n (secondes avant explosion ET portee)", bomb_n, 1, 8);
            printf("OK. Entree pour continuer...");
            getchar();
        } else if (choice == 4) {
            display_best_scores();
            printf("\nEntree pour continuer...");
            getchar();
        } else if (choice == 5) {
            break;
        }
    }

    printf("Merci d'avoir joue !\n");
    return EXIT_SUCCESS;
}
