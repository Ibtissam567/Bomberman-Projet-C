# Bomberman — Projet Langage C

Jeu de type Bomberman en C (console), conforme au sujet ENSEM 2025–2026 :

1. **Mono-bomberman** — arène, déplacement, bombes, score, sauvegarde
2. **Multi-bomberman** — deux joueurs simultanés
3. **Version améliorée** — couleurs, pause, bonus `+`, sons, menu, paramètre `n`

Le code source est dans le dossier [`bomberman/`](bomberman/).

## Compilation (Windows / CodeBlocks)

1. Ouvrir `bomberman/Bomberman.cbp` dans CodeBlocks
2. **Build → Build and run** (F9)

Ou en ligne de commande (MinGW) :

```bat
cd bomberman
compiler.bat
```

## Contrôles

| Joueur   | Déplacement | Bombe  |
|----------|-------------|--------|
| Joueur 1 | Z Q S D     | W      |
| Joueur 2 | Flèches     | Espace |

`P` = pause, `ESC` = quitter.

## Règles

- Arène chargée depuis `arene.txt` (`#` murs, `X` obstacles, `B` joueurs, `o` bombes)
- Obstacles et joueurs placés aléatoirement, sans superposition
- Une bombe à la fois, posée **devant** le joueur
- Explosion après **n** secondes, portée **n**
- Score = `(obstacles détruits × 50) / bombes posées`
- Fichier `scores.txt` : `pseudo;score;date`
