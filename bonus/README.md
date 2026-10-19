# Bonus

Idées pour aller plus loin, une fois les trois étapes en place. Pas de code pour l'instant : on le prépare quand le choix est fait.

## Batterie

Pour jouer sans câble : batterie 18650 de marque + support, module de charge TP4056 **avec protection** (USB-C), convertisseur élévateur MT3608 réglé à 5,1 V vers VIN, et un interrupteur à bascule sur la sortie. L'anneau à luminosité 40 et l'ampli à volume 30 tirent environ 150 à 250 mA : compter 8 à 12 heures de jeu avec une 2500 mAh. Batterie dans un compartiment fermé par une vis.

## Vrais sons et histoires sur carte SD

Le lecteur **DFPlayer Mini** (3 à 6 €) lit des MP3 depuis une carte micro SD et pilote directement un haut-parleur. On peut y mettre des bruitages d'animaux (un bouton = un animal), des comptines ou de courtes histoires. Il se branche sur un port série de l'ESP32 (RX2/TX2, GPIO16/17), avec une résistance de 1 kΩ sur la ligne RX du DFPlayer.

## Réglage du volume

Un potentiomètre de 10 kΩ sur GPIO34 (entrée analogique seule), lu dans la boucle pour remplacer la constante `VOLUME`. Un bouton de volume accessible aux parents seulement, à l'arrière du boîtier, évite les mauvaises surprises.

## Mise en veille

Après quelques minutes sans appui, éteindre l'anneau et l'écran puis passer l'ESP32 en sommeil profond, réveil par le bouton rouge (GPIO32 est une broche RTC, compatible avec le réveil). Indispensable avec la batterie.

## Autres jeux possibles

- **Réflexes** : une couleur s'allume au hasard, il faut appuyer le plus vite possible, temps affiché à l'écran.
- **Devine la note** : le module joue une note, l'enfant cherche le bouton qui la joue.
- **Minuteur de brossage de dents** : deux minutes d'anneau qui se vide, avec une musique à la fin.
