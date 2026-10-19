// Module ludique pour enfants - étape 3 : jeu Simon avec écran
// Accueil : bouton rouge = jeu Simon, bouton vert = piano libre.
// Simon : le module montre une suite de couleurs, l'enfant la répète.
// À chaque réussite, la suite s'allonge d'une couleur et accélère.
// Le meilleur score reste en mémoire même débranché.
#include <Arduino.h>
#include <Wire.h>
#include <Preferences.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include "reglages.h"
#include "son.h"

Adafruit_NeoPixel anneau(NB_LEDS, BROCHE_NEOPIXELS, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 ecran(128, 64, &Wire, -1);
Preferences memoire;
bool ecranPresent = false;

static const uint32_t COULEURS[NB_BOUTONS] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFB000};
static const char* NOMS[NB_BOUTONS] = {"ROUGE", "VERT", "BLEU", "JAUNE"};
static const uint16_t NOTES[NB_BOUTONS] = {NOTE_DO4, NOTE_MI4, NOTE_SOL4, NOTE_DO5};

static const int LONGUEUR_MAX = 100;
uint8_t suite[LONGUEUR_MAX];
int meilleurScore = 0;
uint16_t teinteArcEnCiel = 0;

// ---------- Boutons ----------

static const unsigned long ANTI_REBOND_MS = 30;
bool etatStable[NB_BOUTONS];
bool derniereLecture[NB_BOUTONS];
unsigned long dernierChangement[NB_BOUTONS];

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

// Renvoie le bouton appuyé (0 à 3), ou -1 si rien avant le délai.
int attendreAppui(unsigned long delaiMs) {
  unsigned long debut = millis();
  while (millis() - debut < delaiMs) {
    for (int i = 0; i < NB_BOUTONS; i++) {
      if (vientDEtreAppuye(i)) return i;
    }
    delay(2);
  }
  return -1;
}

void attendreRelache(int i) {
  while (digitalRead(BROCHES_BOUTONS[i]) == LOW) delay(5);
  etatStable[i] = derniereLecture[i] = false;
}

// ---------- Lumières ----------

void remplir(uint32_t couleur) {
  for (int i = 0; i < NB_LEDS; i++) anneau.setPixelColor(i, couleur);
  anneau.show();
}

// Allume seulement le quart de l'anneau qui correspond au bouton.
void allumerQuart(int bouton) {
  int quart = NB_LEDS / NB_BOUTONS;
  anneau.clear();
  for (int i = 0; i < quart; i++) anneau.setPixelColor(bouton * quart + i, COULEURS[bouton]);
  anneau.show();
}

void afficherArcEnCiel() {
  for (int i = 0; i < NB_LEDS; i++) {
    uint16_t teinte = teinteArcEnCiel + i * 65536L / NB_LEDS;
    anneau.setPixelColor(i, anneau.gamma32(anneau.ColorHSV(teinte)));
  }
  anneau.show();
  teinteArcEnCiel += 256;
}

// ---------- Écran ----------

void ecrireCentre(const char* texte, int y, int taille) {
  int16_t x1, y1;
  uint16_t l, h;
  ecran.setTextSize(taille);
  ecran.getTextBounds(texte, 0, 0, &x1, &y1, &l, &h);
  ecran.setCursor((128 - l) / 2, y);
  ecran.print(texte);
}

void afficher(const char* titre, const char* ligne1, const char* ligne2 = "") {
  Serial.printf("[Écran] %s | %s | %s\n", titre, ligne1, ligne2);
  if (!ecranPresent) return;
  ecran.clearDisplay();
  ecran.setTextColor(SSD1306_WHITE);
  ecrireCentre(titre, 0, 2);
  ecrireCentre(ligne1, 28, 1);
  ecrireCentre(ligne2, 44, 1);
  ecran.display();
}

void afficherScore(int score) {
  char ligne[24];
  snprintf(ligne, sizeof(ligne), "Record : %d", meilleurScore);
  if (!ecranPresent) {
    Serial.printf("[Écran] Score %d, %s\n", score, ligne);
    return;
  }
  ecran.clearDisplay();
  ecran.setTextColor(SSD1306_WHITE);
  ecrireCentre("SCORE", 0, 1);
  char nombre[8];
  snprintf(nombre, sizeof(nombre), "%d", score);
  ecrireCentre(nombre, 14, 4);
  ecrireCentre(ligne, 54, 1);
  ecran.display();
}

// ---------- Sons d'ambiance ----------

void sonGagne() {
  static const uint16_t notes[] = {NOTE_DO5, NOTE_SOL4, NOTE_DO5};
  static const uint16_t durees[] = {80, 80, 160};
  sonMelodie(notes, durees, 3);
}

void sonPerdu() {
  static const uint16_t notes[] = {NOTE_SOL4, NOTE_FA4, NOTE_RE4, NOTE_DO4};
  static const uint16_t durees[] = {250, 250, 250, 600};
  for (int i = 0; i < 4; i++) {
    remplir(i % 2 ? 0 : 0xFF0000);
    sonMelodie(&notes[i], &durees[i], 1);
  }
  remplir(0);
}

void sonRecord() {
  static const uint16_t notes[] = {NOTE_DO4, NOTE_MI4, NOTE_SOL4, NOTE_DO5, 0, NOTE_SOL4, NOTE_DO5};
  static const uint16_t durees[] = {120, 120, 120, 250, 80, 120, 500};
  for (int i = 0; i < 7; i++) {
    afficherArcEnCiel();
    sonMelodie(&notes[i], &durees[i], 1);
  }
  remplir(0);
}

// ---------- Jeu Simon ----------

// Montre une couleur : quart allumé + note, puis courte pause.
void montrer(int bouton, int dureeMs) {
  allumerQuart(bouton);
  sonJouer(NOTES[bouton]);
  delay(dureeMs);
  sonArreter();
  anneau.clear();
  anneau.show();
  delay(dureeMs / 3);
}

void jouerSimon() {
  afficher("SIMON", "Regarde bien...", "puis repete !");
  delay(1500);
  int longueur = 0;

  for (;;) {
    if (longueur < LONGUEUR_MAX) suite[longueur++] = esp_random() % NB_BOUTONS;
    afficherScore(longueur - 1);

    // Plus la suite est longue, plus elle va vite (de 600 ms à 200 ms par couleur).
    int duree = max(200, 600 - (longueur - 1) * 30);
    delay(600);
    for (int i = 0; i < longueur; i++) montrer(suite[i], duree);

    // À l'enfant de jouer.
    for (int i = 0; i < longueur; i++) {
      int appui = attendreAppui(DELAI_REPONSE_MS);
      if (appui < 0 || appui != suite[i]) {
        Serial.printf("Perdu : attendu %s, reçu %s\n", NOMS[suite[i]], appui < 0 ? "rien" : NOMS[appui]);
        sonArreter();
        sonPerdu();
        int score = longueur - 1;
        if (score > meilleurScore) {
          meilleurScore = score;
          memoire.putInt("record", meilleurScore);
          afficher("RECORD !", "Meilleur score !");
          sonRecord();
        }
        afficherScore(score);
        delay(3000);
        return;
      }
      allumerQuart(appui);
      sonJouer(NOTES[appui]);
      attendreRelache(appui);
      sonArreter();
      anneau.clear();
      anneau.show();
    }
    delay(300);
    sonGagne();
  }
}

// ---------- Piano libre ----------

void jouerPiano() {
  afficher("PIANO", "Joue ce que tu veux !", "");
  unsigned long dernierAppui = millis();
  int boutonTenu = -1;
  while (millis() - dernierAppui < DELAI_PIANO_MS) {
    for (int i = 0; i < NB_BOUTONS; i++) {
      if (vientDEtreAppuye(i)) {
        boutonTenu = i;
        dernierAppui = millis();
        remplir(COULEURS[i]);
        sonJouer(NOTES[i]);
        afficher("PIANO", NOMS[i], "");
      }
    }
    if (boutonTenu >= 0 && !etatStable[boutonTenu]) {
      boutonTenu = -1;
      sonArreter();
      remplir(0);
    }
    delay(5);
  }
  sonArreter();
}

// ---------- Programme principal ----------

void afficherAccueil() {
  char ligne[24];
  snprintf(ligne, sizeof(ligne), "Record : %d", meilleurScore);
  afficher("COUCOU !", "R: Simon   V: Piano", ligne);
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

  Wire.begin(BROCHE_SDA, BROCHE_SCL);
  ecranPresent = ecran.begin(SSD1306_SWITCHCAPVCC, ADRESSE_ECRAN);
  if (!ecranPresent) Serial.println("Écran introuvable : vérifie le câblage et l'adresse (0x3C ou 0x3D)");

  memoire.begin("simon", false);
  meilleurScore = memoire.getInt("record", 0);

  sonDemarrer(BROCHE_I2S_BCLK, BROCHE_I2S_LRC, BROCHE_I2S_DIN, VOLUME);
  sonRecord();
  Serial.println("Module enfants - étape 3 prête");
  afficherAccueil();
}

void loop() {
  afficherArcEnCiel();
  for (int i = 0; i < NB_BOUTONS; i++) {
    if (vientDEtreAppuye(i)) {
      attendreRelache(i);
      remplir(0);
      if (i == 0) jouerSimon();
      else if (i == 1) jouerPiano();
      afficherAccueil();
    }
  }
  delay(20);
}
