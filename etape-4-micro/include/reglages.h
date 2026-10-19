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

// Amplificateur I2S MAX98357A.
#define BROCHE_I2S_BCLK 26
#define BROCHE_I2S_LRC  25
#define BROCHE_I2S_DIN  27

// Volume logiciel, de 0 à 100. Commencer bas : un haut-parleur de 3 W
// près de petites oreilles, c'est fort. 30 est un bon départ.
#define VOLUME 30

// Écran OLED SSD1306 128x64 en I2C (broches I2C par défaut de l'ESP32).
#define BROCHE_SDA     21
#define BROCHE_SCL     22
#define ADRESSE_ECRAN  0x3C   // certains écrans sont en 0x3D

// Jeu Simon.
#define DELAI_REPONSE_MS 5000  // temps laissé pour appuyer avant de perdre
#define DELAI_PIANO_MS   15000 // retour à l'accueil sans appui en mode piano

// Micro I2S INMP441 (second périphérique I2S de l'ESP32, le premier sert à l'ampli).
// Broche L/R du micro à GND : le micro parle sur le canal gauche.
#define BROCHE_MICRO_SCK 14
#define BROCHE_MICRO_WS  4
#define BROCHE_MICRO_SD  35   // entrée seule, parfait pour une donnée qui arrive
// Si le micro ne capte rien, essayer 1 : certaines versions de l'ESP32 inversent
// les canaux gauche et droit.
#define MICRO_CANAL_DROIT 0
// Amplification logicielle du micro (1 à 8). Monter si le perroquet parle trop bas.
#define MICRO_GAIN 2

// Perroquet : durée maximale d'un enregistrement.
#define DUREE_ENREGISTREMENT_MS 2500  // 80 Ko de mémoire ; au-delà de 3 s, la place manque
#define DELAI_PERROQUET_MS 20000  // retour à l'accueil sans appui
// Disco : niveau sonore (0 à 100) à partir duquel un bruit compte comme un clap.
#define SEUIL_CLAP 60
