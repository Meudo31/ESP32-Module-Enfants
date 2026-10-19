// Micro I2S INMP441, sur le second périphérique I2S de l'ESP32.
#pragma once
#include <Arduino.h>

#define MICRO_FREQUENCE 16000  // échantillons par seconde

void microDemarrer(uint8_t brocheSck, uint8_t brocheWs, uint8_t brocheSd);
// Le micro ne tourne que quand on en a besoin : microActiver(true) vide les
// anciens échantillons et repart de zéro.
void microActiver(bool actif);
// Lit jusqu'à nombre échantillons 16 bits, attend s'il le faut. Renvoie le nombre lu.
size_t microLire(int16_t* destination, size_t nombre);
// Niveau sonore actuel de 0 (silence) à 100 (très fort), sur environ 16 ms.
int microNiveau();
