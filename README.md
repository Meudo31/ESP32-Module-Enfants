# Module ludique pour enfants

Une boîte à boutons qui s'allume, chante et joue au Simon avec les enfants. Le projet avance par étapes, du plus simple au plus complet ; chaque étape est un projet PlatformIO autonome et peut faire l'objet d'une vidéo.

| Étape | Dossier | Ce que ça fait |
| --- | --- | --- |
| 1 | [etape-1-lumieres](etape-1-lumieres) | 4 boutons d'arcade, anneau NeoPixel : chaque bouton remplit l'anneau de sa couleur, arc-en-ciel au repos |
| 2 | [etape-2-sons](etape-2-sons) | Ajoute un haut-parleur : piano de couleurs, jingle au démarrage, « Au clair de la lune » en tenant rouge + jaune |
| 3 | [etape-3-jeu-simon](etape-3-jeu-simon) | Ajoute un écran : jeu Simon avec score, record gardé en mémoire, et mode piano libre |
| Bonus | [bonus](bonus) | Batterie, vrais sons et histoires sur carte SD, réglage du volume, mise en veille |

Toutes les étapes utilisent la même carte, **ESP32 DevKit V1 (ESP32-WROOM-32, 30 broches)**, et le même brochage : on ajoute des pièces sans rien recâbler. Pas de Wi-Fi, donc pas de secret : les réglages (broches, luminosité, volume) sont dans `include/reglages.h` et peuvent être publiés.

## Brochage commun

| Pièce | Broche ESP32 |
| --- | --- |
| Bouton rouge / vert / bleu / jaune (autre patte à GND) | GPIO32 / GPIO33 / GPIO18 / GPIO19 |
| Anneau NeoPixel, DIN (via résistance 330 Ω) | GPIO13 |
| MAX98357A : BCLK / LRC / DIN | GPIO26 / GPIO25 / GPIO27 |
| Écran OLED : SDA / SCL | GPIO21 / GPIO22 |
| Alimentation 5 V de l'anneau et de l'ampli | VIN (5 V de l'USB) |

## Liste d'achat par étape

Prix approximatifs ; chaque étape réutilise ce qui a été acheté avant.

### Étape 1 (environ 18 à 28 €)

| Pièce | Prix approx. | Où chercher |
| --- | --- | --- |
| ESP32 DevKit V1 (ESP32-WROOM-32, 30 broches, USB-C ou micro-USB) | 5 à 7 € | [Amazon](https://www.amazon.fr/s?k=esp32+devkit+v1) · [AliExpress](https://fr.aliexpress.com/w/wholesale-esp32-devkit-v1.html) |
| Anneau NeoPixel 12 LED WS2812B (environ 50 mm) | 3 à 5 € | [Amazon](https://www.amazon.fr/s?k=anneau+ws2812b+12+led) · [AliExpress](https://fr.aliexpress.com/w/wholesale-anneau-ws2812b-12-led.html) |
| 4 boutons d'arcade 30 mm de couleurs (rouge, vert, bleu, jaune) | 5 à 10 € le lot | [Amazon](https://www.amazon.fr/s?k=bouton+arcade+30mm) · [AliExpress](https://fr.aliexpress.com/w/wholesale-bouton-arcade-30mm.html) |
| Résistance 330 Ω et condensateur 1000 µF 10 V | moins d'1 € | |
| Fils Dupont, cosses 2,8 mm pour les boutons, plaque à trous | 3 € | |
| Boîtier : boîte en bois ou en plastique d'environ 15 × 10 × 6 cm, ou impression 3D | 2 à 5 € | |

### Étape 2 (environ 5 à 9 € en plus)

| Pièce | Prix approx. | Où chercher |
| --- | --- | --- |
| Amplificateur I2S MAX98357A | 3 à 5 € | [Amazon](https://www.amazon.fr/s?k=max98357a) · [AliExpress](https://fr.aliexpress.com/w/wholesale-max98357a.html) |
| Haut-parleur 4 Ω 3 W, 40 mm environ | 2 à 4 € | [Amazon](https://www.amazon.fr/s?k=haut+parleur+4+ohm+3w+40mm) · [AliExpress](https://fr.aliexpress.com/w/wholesale-haut-parleur-4-ohm-3w-40mm.html) |

### Étape 3 (environ 3 à 5 € en plus)

| Pièce | Prix approx. | Où chercher |
| --- | --- | --- |
| Écran OLED 0,96 pouce SSD1306 I2C 128 × 64 (4 broches) | 3 à 5 € | [Amazon](https://www.amazon.fr/s?k=oled+ssd1306+0.96+i2c) · [AliExpress](https://fr.aliexpress.com/w/wholesale-oled-ssd1306-0.96-i2c.html) |

### Bonus

Voir [bonus/README.md](bonus/README.md) pour le détail.

| Pièce | Prix approx. |
| --- | --- |
| Batterie 18650 de marque + support, TP4056 protégé, convertisseur 5 V MT3608, interrupteur | 8 à 12 € |
| Lecteur DFPlayer Mini + carte micro SD (vrais bruitages, comptines, histoires) | 3 à 6 € |
| Potentiomètre 10 kΩ + bouton (volume) | 1 € |

## Mise en route

Chaque dossier a son propre README avec le câblage et les essais. Dans tous les cas : brancher la carte en USB, puis `pio run -t upload` et `pio device monitor` depuis le dossier de l'étape.

## Sécurité enfants

- Luminosité limitée à 40 sur 255 et volume à 30 sur 100 par défaut, réglables dans `reglages.h`.
- Aucune pièce électronique accessible : tout est vissé ou collé dans le boîtier, les boutons d'arcade sont les seuls éléments à toucher.
- Avec la batterie du bonus, prendre une 18650 de marque et un TP4056 avec protection, et garder la batterie dans un compartiment fermé par une vis.
