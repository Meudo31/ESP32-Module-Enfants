// Brochage et réglages du module. Pas de secret ici : ce fichier peut être publié.
#pragma once

// 4 boutons d'arcade, reliés entre la broche et GND (tirage interne activé).
// Ordre : rouge, vert, bleu, jaune.
#define NB_BOUTONS 4
static const uint8_t BROCHES_BOUTONS[NB_BOUTONS] = {32, 33, 18, 19};

// Anneau NeoPixel (WS2812B) : entrée DIN via une résistance de 330 ohms.
#define BROCHE_NEOPIXELS 13
#define NB_LEDS          12

// Luminosité maximale (0 à 255). 40 suffit largement à l'intérieur,
// ménage les yeux des enfants et limite le courant tiré sur l'USB.
#define LUMINOSITE_MAX 40
