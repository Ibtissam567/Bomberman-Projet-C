#ifndef SCORE_H
#define SCORE_H

#include "config.h"

typedef struct {
    char pseudo[MAX_PSEUDO];
    int score;
    char date[20];
} ScoreEntry;

/* Calcule le score selon la formule imposée :
 *   score = (obstacles_detruits * 50) / nombre_de_bombes_posees
 * Renvoie 0 si aucune bombe n'a été posée (évite la division par zéro). */
int compute_score(int obstacles_detruits, int bombes_posees);

/* Ajoute une entrée (pseudo, score, date du jour) au fichier des meilleurs
 * scores, puis réécrit le fichier trié par score décroissant en ne gardant
 * que les MAX_SCORES_KEPT meilleurs. */
void save_score(const char *pseudo, int score);

/* Affiche le tableau des meilleurs scores à l'écran. */
int  score_load(ScoreEntry entries[], int max_entries);
void display_best_scores(void);

#endif /* SCORE_H */
