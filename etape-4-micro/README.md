# Étape 4 : micro, perroquet et disco

On ajoute un micro I2S **INMP441**. L'accueil propose maintenant quatre jeux, un par bouton :

- **Rouge, Simon** et **vert, piano libre** : comme à l'étape 3.
- **Bleu, perroquet** : l'enfant tient le bouton rouge et parle (2,5 secondes maximum, l'anneau se remplit pendant l'enregistrement). Ensuite, vert rejoue la voix normale, bleu en **voix de souris**, jaune en **voix de géant**, et rouge réenregistre. Retour à l'accueil après 20 secondes sans appui.
- **Jaune, disco** : l'anneau monte et descend avec le volume de la voix ou de la musique, et chaque **clap** fait flasher l'anneau en blanc, change les couleurs et s'affiche à l'écran avec le compte. N'importe quel bouton ramène à l'accueil.

L'enregistrement reste en mémoire vive, rien n'est gardé une fois sorti du perroquet. Le code compile avec PlatformIO mais n'a pas encore été testé sur une vraie carte.

## Câblage

Tout le câblage des étapes 1 à 3 reste en place. On ajoute :

| De (INMP441) | Vers |
| --- | --- |
| VDD | 3V3 de l'ESP32 (**pas** 5 V) |
| GND | GND |
| L/R | GND (le micro parle sur le canal gauche) |
| SCK | GPIO14 |
| WS | GPIO4 |
| SD | GPIO35 |

Place le micro loin du haut-parleur, face à un petit trou dans le boîtier, et colle-le sur un morceau de mousse pour qu'il n'entende pas les vibrations des boutons.

## Mise en route

1. `pio run -t upload` puis `pio device monitor` depuis ce dossier.
2. Bouton jaune (disco) : parle et tape dans tes mains, l'anneau doit réagir.
3. Bouton bleu (perroquet) : tiens rouge, dis « Bonjour ! », lâche, puis appuie sur bleu.

## Réglages

Dans `include/reglages.h` :

- `MICRO_GAIN` (1 à 8) : à monter si le perroquet parle trop bas, à baisser s'il sature.
- `SEUIL_CLAP` (0 à 100) : à monter si la disco voit des claps partout, à baisser si elle les rate.
- `DUREE_ENREGISTREMENT_MS` : 2500 par défaut. Chaque seconde prend 32 Ko de mémoire ; au-delà de 3 secondes, l'ESP32 n'a plus la place d'un seul bloc.
- Les vitesses des voix (1,6 pour la souris, 0,65 pour le géant) sont dans `jouerPerroquet()` de `main.cpp`.

## Points à vérifier ensemble

- **La disco ne réagit pas du tout** : mets `MICRO_CANAL_DROIT` à 1. Selon la version de l'ESP32, les canaux gauche et droit sont parfois inversés.
- **Effet Larsen ou sifflement** : le micro n'est jamais écouté en même temps que le haut-parleur joue, donc ça ne devrait pas arriver. S'il y a un sifflement pendant la disco, il vient d'une masse mal reliée.
- **Le niveau de la disco reste au maximum ou au minimum** : le calcul du niveau dans `microNiveau()` (fichier `micro.cpp`) part d'une plage de 40 à 80 dB ; on l'ajustera ensemble avec les valeurs affichées.
