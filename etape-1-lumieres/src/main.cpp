// Module ludique pour enfants - étape 1 : boutons et lumières
// Au repos, l'anneau tourne en arc-en-ciel. Chaque bouton remplit l'anneau
// de sa couleur depuis son quart, puis la couleur s'efface quand on lâche.
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "reglages.h"

Adafruit_NeoPixel anneau(NB_LEDS, BROCHE_NEOPIXELS, NEO_GRB + NEO_KHZ800);

// Couleur de chaque bouton, dans l'ordre de BROCHES_BOUTONS.
static const uint32_t COULEURS[NB_BOUTONS] = {
  0xFF0000,  // rouge
  0x00FF00,  // vert
  0x0000FF,  // bleu
  0xFFB000,  // jaune
};

static const unsigned long ANTI_REBOND_MS = 30;
static const unsigned long DELAI_REPOS_MS = 5000;  // retour à l'arc-en-ciel

bool etatStable[NB_BOUTONS];
bool derniereLecture[NB_BOUTONS];
unsigned long dernierChangement[NB_BOUTONS];

int boutonActif = -1;           // bouton dont la couleur est affichée
float niveau = 0.0f;            // 0 = éteint, 1 = pleine couleur
int ledsRemplies = 0;           // progression du remplissage depuis le quart du bouton
unsigned long dernierAppui = 0;
uint16_t teinteArcEnCiel = 0;

// Renvoie true une seule fois, au moment où le bouton est enfoncé.
bool vientDEtreAppuye(int i) {
  bool lecture = digitalRead(BROCHES_BOUTONS[i]) == LOW;
  unsigned long maintenant = millis();
  if (lecture != derniereLecture[i]) {
    derniereLecture[i] = lecture;
    dernierChangement[i] = maintenant;
  }
  if (maintenant - dernierChangement[i] > ANTI_REBOND_MS && lecture != etatStable[i]) {
    etatStable[i] = lecture;
    return lecture;
  }
  return false;
}

uint32_t attenuer(uint32_t couleur, float facteur) {
  uint8_t r = ((couleur >> 16) & 0xFF) * facteur;
  uint8_t v = ((couleur >> 8) & 0xFF) * facteur;
  uint8_t b = (couleur & 0xFF) * facteur;
  return anneau.Color(r, v, b);
}

void afficherArcEnCiel() {
  for (int i = 0; i < NB_LEDS; i++) {
    uint16_t teinte = teinteArcEnCiel + i * 65536L / NB_LEDS;
    anneau.setPixelColor(i, anneau.gamma32(anneau.ColorHSV(teinte)));
  }
  teinteArcEnCiel += 256;
}

void afficherCouleurBouton() {
  // Le remplissage part du milieu du quart du bouton et s'étend des deux côtés.
  int quart = NB_LEDS / NB_BOUTONS;
  int centre = boutonActif * quart + quart / 2;
  anneau.clear();
  for (int d = 0; d <= ledsRemplies / 2; d++) {
    uint32_t c = attenuer(COULEURS[boutonActif], niveau);
    anneau.setPixelColor((centre + d) % NB_LEDS, c);
    anneau.setPixelColor((centre - d + NB_LEDS) % NB_LEDS, c);
  }
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NB_BOUTONS; i++) {
    pinMode(BROCHES_BOUTONS[i], INPUT_PULLUP);
    etatStable[i] = derniereLecture[i] = false;
    dernierChangement[i] = 0;
  }
  anneau.begin();
  anneau.setBrightness(LUMINOSITE_MAX);
  anneau.show();
  Serial.println("Module enfants - étape 1 prête");
}

void loop() {
  for (int i = 0; i < NB_BOUTONS; i++) {
    if (vientDEtreAppuye(i)) {
      Serial.printf("Bouton %d\n", i + 1);
      boutonActif = i;
      niveau = 1.0f;
      ledsRemplies = 0;
      dernierAppui = millis();
    }
  }

  if (boutonActif >= 0) {
    if (ledsRemplies < NB_LEDS) ledsRemplies++;
    // Tant que le bouton est tenu, la couleur reste ; sinon elle s'efface doucement.
    if (!etatStable[boutonActif]) niveau = max(0.0f, niveau - 0.03f);
    afficherCouleurBouton();
    if (niveau == 0.0f && millis() - dernierAppui > DELAI_REPOS_MS) boutonActif = -1;
  } else {
    afficherArcEnCiel();
  }

  anneau.show();
  delay(20);
}
