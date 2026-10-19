# Étape 2 : boutons, lumières et sons

On ajoute un petit amplificateur I2S et un haut-parleur. Chaque bouton joue sa note (do, mi, sol, do aigu) en allumant l'anneau de sa couleur : un piano de couleurs dont les notes sonnent toujours bien ensemble. Au démarrage, un petit jingle fait le tour des couleurs. En tenant **rouge et jaune ensemble pendant une seconde**, le module joue « Au clair de la lune » en montrant la couleur de chaque note.

Les sons sont calculés par l'ESP32 lui-même, il n'y a aucun fichier audio à préparer. Le code compile avec PlatformIO mais n'a pas encore été testé sur une vraie carte.

## Câblage

Tout le câblage de l'étape 1 reste en place. On ajoute :

| De (MAX98357A) | Vers |
| --- | --- |
| VIN | VIN de l'ESP32 (5 V) |
| GND | GND |
| BCLK | GPIO26 |
| LRC | GPIO25 |
| DIN | GPIO27 |
| SD et GAIN | rien (laisser en l'air) |
| Sorties + et - | haut-parleur 4 Ω |

Laisser GAIN en l'air donne un gain de 9 dB, largement assez. SD en l'air mélange gauche et droite, ce qui convient au code.

## Mise en route

1. `pio run -t upload` puis `pio device monitor` depuis ce dossier.
2. Au démarrage : jingle de 4 notes avec les 4 couleurs, puis arc-en-ciel.
3. Appuie sur les boutons : chaque note tient tant que le bouton est enfoncé.
4. Tiens rouge et jaune ensemble une seconde : la comptine démarre.

## Réglages

Dans `include/reglages.h` : `VOLUME` (0 à 100, 30 par défaut) et `LUMINOSITE_MAX`. Le fichier `src/son.cpp` contient le petit synthétiseur, réutilisé tel quel à l'étape 3 ; on peut y changer le timbre (le mélange sinus + harmonique 3 dans `sonDemarrer`).

## Points à vérifier ensemble

- **Aucun son** : vérifie VIN à 5 V sur l'ampli et l'inversion possible de BCLK et LRC.
- **Grésillement ou bruit de fond** : éloigne les fils du haut-parleur des fils de l'anneau, et vérifie la masse commune. Le condensateur de 1000 µF sur l'alimentation aide aussi.
- **Clic à chaque note** : les montées et descentes de volume (`MONTEE`, `DESCENTE` dans `son.cpp`) peuvent être adoucies.
