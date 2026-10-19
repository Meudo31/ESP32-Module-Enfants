// Module ludique pour enfants - étape 2 : boutons, lumières et sons
// Chaque bouton joue sa note et allume l'anneau de sa couleur : un piano
// de couleurs. Rouge + jaune ensemble pendant une seconde : l'anneau joue
// « Au clair de la lune » en montrant la couleur de chaque note.
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "reglages.h"
#include "son.h"

Adafruit_NeoPixel anneau(NB_LEDS, BROCHE_NEOPIXELS, NEO_GRB + NEO_KHZ800);

static const uint32_t COULEURS[NB_BOUTONS] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFB000};
// Do, mi, sol, do aigu : un accord qui sonne bien quelles que soient les touches.
static const uint16_t NOTES[NB_BOUTONS] = {NOTE_DO4, NOTE_MI4, NOTE_SOL4, NOTE_DO5};

static const unsigned long ANTI_REBOND_MS = 30;
static const unsigned long DELAI_REPOS_MS = 5000;
static const unsigned long DUREE_COMBO_MS = 1000;

bool etatStable[NB_BOUTONS];
bool derniereLecture[NB_BOUTONS];
unsigned long dernierChangement[NB_BOUTONS];

int boutonActif = -1;
float niveau = 0.0f;
unsigned long dernierAppui = 0;
unsigned long debutCombo = 0;
uint16_t teinteArcEnCiel = 0;

// Met à jour l'état des boutons ; renvoie true au moment où le bouton i est enfoncé.
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
  return anneau.Color(((couleur >> 16) & 0xFF) * facteur,
                      ((couleur >> 8) & 0xFF) * facteur,
                      (couleur & 0xFF) * facteur);
}

void remplir(uint32_t couleur) {
  for (int i = 0; i < NB_LEDS; i++) anneau.setPixelColor(i, couleur);
  anneau.show();
}

void afficherArcEnCiel() {
  for (int i = 0; i < NB_LEDS; i++) {
    uint16_t teinte = teinteArcEnCiel + i * 65536L / NB_LEDS;
    anneau.setPixelColor(i, anneau.gamma32(anneau.ColorHSV(teinte)));
  }
  teinteArcEnCiel += 256;
}

// Au clair de la lune : chaque note allume la couleur la plus proche.
void jouerAuClairDeLaLune() {
  static const uint16_t notes[] = {
    NOTE_DO4, NOTE_DO4, NOTE_DO4, NOTE_RE4, NOTE_MI4, NOTE_RE4,
    NOTE_DO4, NOTE_MI4, NOTE_RE4, NOTE_RE4, NOTE_DO4};
  static const uint16_t durees[] = {
    400, 400, 400, 400, 800, 800,
    400, 400, 400, 400, 1200};
  static const int nombre = sizeof(notes) / sizeof(notes[0]);
  Serial.println("Mélodie : Au clair de la lune");
  for (int i = 0; i < nombre; i++) {
    uint32_t couleur = notes[i] == NOTE_DO4 ? COULEURS[0] : notes[i] == NOTE_RE4 ? COULEURS[3] : COULEURS[1];
    remplir(couleur);
    sonMelodie(&notes[i], &durees[i], 1);
  }
  remplir(0);
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
  sonDemarrer(BROCHE_I2S_BCLK, BROCHE_I2S_LRC, BROCHE_I2S_DIN, VOLUME);

  // Petit jingle de démarrage.
  static const uint16_t jingle[] = {NOTE_DO4, NOTE_MI4, NOTE_SOL4, NOTE_DO5};
  static const uint16_t durees[] = {120, 120, 120, 300};
  for (int i = 0; i < 4; i++) {
    remplir(COULEURS[i]);
    sonMelodie(&jingle[i], &durees[i], 1);
  }
  remplir(0);
  Serial.println("Module enfants - étape 2 prête");
}

void loop() {
  for (int i = 0; i < NB_BOUTONS; i++) {
    if (vientDEtreAppuye(i)) {
      Serial.printf("Bouton %d\n", i + 1);
      boutonActif = i;
      niveau = 1.0f;
      dernierAppui = millis();
      sonJouer(NOTES[i]);
    }
  }

  // Combo rouge + jaune tenus ensemble : mélodie.
  if (etatStable[0] && etatStable[3]) {
    if (!debutCombo) debutCombo = millis();
    if (millis() - debutCombo > DUREE_COMBO_MS) {
      sonArreter();
      jouerAuClairDeLaLune();
      debutCombo = 0;
      boutonActif = -1;
      // Attend que les deux boutons soient lâchés pour ne pas relancer.
      while (digitalRead(BROCHES_BOUTONS[0]) == LOW || digitalRead(BROCHES_BOUTONS[3]) == LOW) delay(10);
      for (int i = 0; i < NB_BOUTONS; i++) etatStable[i] = derniereLecture[i] = false;
      return;
    }
  } else {
    debutCombo = 0;
  }

  if (boutonActif >= 0) {
    if (!etatStable[boutonActif]) {
      // Plus aucun bouton tenu : la note s'arrête et la couleur s'efface.
      bool unBoutonTenu = false;
      for (int i = 0; i < NB_BOUTONS; i++) unBoutonTenu |= etatStable[i];
      if (!unBoutonTenu) sonArreter();
      niveau = max(0.0f, niveau - 0.03f);
    }
    remplir(attenuer(COULEURS[boutonActif], niveau));
    if (niveau == 0.0f && millis() - dernierAppui > DELAI_REPOS_MS) boutonActif = -1;
  } else {
    afficherArcEnCiel();
    anneau.show();
  }
  delay(20);
}
