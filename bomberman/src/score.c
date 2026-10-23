#include "score.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int compute_score(int obstacles_detruits, int bombes_posees) {
    if (bombes_posees == 0) return 0;
    return (obstacles_detruits * 50) / bombes_posees;
}

int score_load(ScoreEntry entries[], int max_entries) {
    FILE *f = fopen(SCORES_FILE, "r");
    if (!f) return 0;

    int count = 0;
    while (count < max_entries &&
           fscanf(f, "%31[^;];%d;%19[^\n]\n",
                  entries[count].pseudo, &entries[count].score, entries[count].date) == 3) {
        count++;
    }
    fclose(f);
    return count;
}

static void write_scores(ScoreEntry entries[], int count) {
    FILE *f = fopen(SCORES_FILE, "w");
    if (!f) {
        fprintf(stderr, "Erreur : impossible d'ecrire le fichier de scores '%s'\n", SCORES_FILE);
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(f, "%s;%d;%s\n", entries[i].pseudo, entries[i].score, entries[i].date);
    }
    fclose(f);
}

void save_score(const char *pseudo, int score) {
    ScoreEntry entries[MAX_SCORES_KEPT + 1];
    int count = score_load(entries, MAX_SCORES_KEPT);

    ScoreEntry new_entry;
    strncpy(new_entry.pseudo, pseudo, MAX_PSEUDO - 1);
    new_entry.pseudo[MAX_PSEUDO - 1] = '\0';
    new_entry.score = score;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(new_entry.date, sizeof(new_entry.date), "%d/%m/%Y", tm_info);

    entries[count] = new_entry;
    count++;

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            if (entries[j].score < entries[j + 1].score) {
                ScoreEntry tmp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = tmp;
            }
        }
    }

    if (count > MAX_SCORES_KEPT) count = MAX_SCORES_KEPT;
    write_scores(entries, count);
}

void display_best_scores(void) {
    ScoreEntry entries[MAX_SCORES_KEPT];
    int count = score_load(entries, MAX_SCORES_KEPT);

    printf("\n===== Meilleurs scores (pseudo + score + date) =====\n");
    if (count == 0) {
        printf("(aucun score enregistre pour le moment)\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        printf("%2d. %-16s %5d pts   (%s)\n", i + 1, entries[i].pseudo,
               entries[i].score, entries[i].date);
    }
}
