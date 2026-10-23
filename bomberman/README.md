# Bomberman — Projet Langage C

Jeu console (Windows / Linux / macOS) conforme au sujet :
mono-bomberman, multi-bomberman simultane, et version amelioree.

## Compilation

### CodeBlocks (recommande sur ce PC)

1. Ouvrir `Bomberman.cbp` avec CodeBlocks.
2. **Build → Build** (ou F9).
3. **Build → Run** (ou Ctrl+F10).

Le fichier `arene.txt` doit rester a cote du projet (il est aussi cherche
depuis `bin/Debug`).

### Ligne de commande (MinGW)

Double-cliquer `compiler.bat`, ou :

```
gcc -Wall -Wextra -std=c11 -Iinclude -o bomberman.exe src\main.c src\game.c src\arena.c src\entities.c src\input.c src\score.c
bomberman.exe
```

## Controles (sujet)

| Joueur   | Haut | Bas | Gauche | Droite | Bombe  |
|----------|------|-----|--------|--------|--------|
| Joueur 1 | Z    | S   | Q      | D      | W      |
| Joueur 2 | ↑    | ↓   | ←      | →      | Espace |

`P` pause, `ESC` quitter.

## Regles

- Arene chargee depuis `arene.txt` (murs `#`).
- Obstacles `X` et joueurs `B` places **aleatoirement**, sans superposition.
- Une seule bombe `o` a la fois par joueur, posee **devant** lui.
- Explosion apres **n** secondes, portee **n** (4 directions). Un mur arrete
  l'explosion ; un obstacle est detruit et arrete la flamme.
- Fin : tous les obstacles detruits (meilleur score gagne) **ou** un joueur
  pris par une bombe (celle de l'adversaire donne la victoire).
- Score = `(obstacles detruits * 50) / bombes posees`.
- Fichier `scores.txt` : `pseudo;score;date`.

## Ameliorations

- Deplacement **simultane** des 2 joueurs (`GetAsyncKeyState` sous Windows).
- Couleurs, explosion `*`, pause, menu, parametre `n` en jeu.
- Bonus `+` (augmente la portee) parfois laisse par un obstacle detruit.
