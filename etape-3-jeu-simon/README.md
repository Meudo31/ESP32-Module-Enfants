# Étape 3 : jeu Simon avec écran

On ajoute un petit écran OLED, et le module devient un vrai jeu.

- **Accueil** : arc-en-ciel, l'écran affiche « COUCOU ! » et le record. Bouton rouge pour jouer au Simon, bouton vert pour le piano libre.
- **Simon** : le module montre une suite de couleurs (quart de l'anneau + note), l'enfant la répète. À chaque réussite, la suite s'allonge d'une couleur et accélère un peu. Erreur ou 5 secondes sans appui : petite musique triste et affichage du score.
- **Record** : le meilleur score reste en mémoire même débranché, avec une fanfare arc-en-ciel quand il est battu.
- **Piano libre** : comme à l'étape 2, avec le nom de la couleur à l'écran. Retour à l'accueil après 15 secondes sans appui.

Le code compile avec PlatformIO mais n'a pas encore été testé sur une vraie carte.

## Câblage

Tout le câblage des étapes 1 et 2 reste en place. On ajoute :

| De (écran OLED) | Vers |
| --- | --- |
| VCC | 3V3 de l'ESP32 |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

## Mise en route

1. `pio run -t upload` puis `pio device monitor` depuis ce dossier.
2. Au démarrage : fanfare, puis l'écran d'accueil. Le moniteur série recopie aussi tout ce qui s'affiche, ce qui permet de tester le jeu même sans écran.
3. Appuie sur rouge, regarde la première couleur, répète-la.

## Réglages

Dans `include/reglages.h` : `DELAI_REPONSE_MS` (temps pour répondre, à allonger pour les plus petits), `DELAI_PIANO_MS`, `ADRESSE_ECRAN`, `VOLUME`, `LUMINOSITE_MAX`. La vitesse de la suite (600 ms par couleur au début, 200 ms au plus rapide) se règle dans `jouerSimon()` de `main.cpp`.

## Points à vérifier ensemble

- **« Écran introuvable » dans le moniteur** : essaie `ADRESSE_ECRAN 0x3D`, et vérifie que l'écran a bien 4 broches (I2C) et pas 7 (SPI).
- **Pas d'accents à l'écran** : la police intégrée de l'écran ne les gère pas, d'où les textes sans accents (« repete »). Le moniteur série, lui, les affiche.
- **Remettre le record à zéro** : effacer la mémoire avec `pio run -t erase`, puis téléverser de nouveau.
