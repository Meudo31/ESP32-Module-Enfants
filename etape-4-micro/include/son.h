// Petit synthétiseur pour l'amplificateur I2S MAX98357A.
// Le son est calculé en continu dans une tâche FreeRTOS : les fonctions
// ci-dessous rendent la main tout de suite.
#pragma once
#include <Arduino.h>

// Notes utiles, en Hz.
#define NOTE_DO4  262
#define NOTE_RE4  294
#define NOTE_MI4  330
#define NOTE_FA4  349
#define NOTE_SOL4 392
#define NOTE_LA4  440
#define NOTE_SI4  494
#define NOTE_DO5  523

void sonDemarrer(uint8_t brocheBclk, uint8_t brocheLrc, uint8_t brocheDin, uint8_t volume);
// Joue une note. dureeMs = 0 : la note tient jusqu'à sonArreter().
void sonJouer(uint16_t frequence, uint16_t dureeMs = 0);
void sonArreter();
// Joue une suite de notes (0 = silence) et attend la fin. Pratique pour les jingles.
void sonMelodie(const uint16_t* notes, const uint16_t* durees, int nombre);
// Rejoue un enregistrement. vitesse 1.0 = voix normale, plus haut = voix de
// souris, plus bas = voix de géant. Rend la main tout de suite.
void sonJouerEchantillons(const int16_t* echantillons, size_t nombre, uint32_t frequence, float vitesse);
bool sonEchantillonsEnCours();
