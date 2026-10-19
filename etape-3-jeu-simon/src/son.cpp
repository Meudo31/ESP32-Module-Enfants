#include "son.h"
#include <driver/i2s.h>
#include <math.h>

static const int FREQ_ECHANTILLONNAGE = 22050;
static const int TAILLE_BLOC = 128;
static const int TAILLE_TABLE = 256;

static int16_t tableSinus[TAILLE_TABLE];
static volatile uint32_t pasPhase = 0;      // vitesse de lecture de la table, 0 = silence demandé
static volatile uint32_t finNoteMs = 0;     // 0 = note tenue
static float amplitudeMax = 0;

// Enveloppe : montée et descente rapides pour éviter les clics.
static float enveloppe = 0;
static const float MONTEE = 0.004f;
static const float DESCENTE = 0.0015f;

static void tacheSon(void*) {
  int16_t bloc[TAILLE_BLOC * 2];  // stéréo : gauche et droite identiques
  uint32_t phase = 0;
  uint32_t pasCourant = 0;
  for (;;) {
    if (finNoteMs && millis() >= finNoteMs) {
      pasPhase = 0;
      finNoteMs = 0;
    }
    uint32_t pasDemande = pasPhase;
    if (pasDemande) pasCourant = pasDemande;  // garde la hauteur pendant la descente
    for (int i = 0; i < TAILLE_BLOC; i++) {
      if (pasDemande) enveloppe = min(1.0f, enveloppe + MONTEE);
      else enveloppe = max(0.0f, enveloppe - DESCENTE);
      phase += pasCourant;
      int16_t e = tableSinus[phase >> 24] * enveloppe * amplitudeMax;
      bloc[2 * i] = e;
      bloc[2 * i + 1] = e;
    }
    size_t ecrits;
    i2s_write(I2S_NUM_0, bloc, sizeof(bloc), &ecrits, portMAX_DELAY);
  }
}

void sonDemarrer(uint8_t brocheBclk, uint8_t brocheLrc, uint8_t brocheDin, uint8_t volume) {
  // Un peu d'harmonique 3 rend le son moins « bip de réveil » qu'un sinus pur.
  for (int i = 0; i < TAILLE_TABLE; i++) {
    float a = 2 * M_PI * i / TAILLE_TABLE;
    tableSinus[i] = 32767 * (0.8f * sinf(a) + 0.2f * sinf(3 * a));
  }
  amplitudeMax = constrain(volume, 0, 100) / 100.0f;

  i2s_config_t config = {};
  config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  config.sample_rate = FREQ_ECHANTILLONNAGE;
  config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  config.dma_buf_count = 4;
  config.dma_buf_len = TAILLE_BLOC;
  config.tx_desc_auto_clear = true;

  i2s_pin_config_t broches = {};
  broches.mck_io_num = I2S_PIN_NO_CHANGE;
  broches.bck_io_num = brocheBclk;
  broches.ws_io_num = brocheLrc;
  broches.data_out_num = brocheDin;
  broches.data_in_num = I2S_PIN_NO_CHANGE;

  i2s_driver_install(I2S_NUM_0, &config, 0, nullptr);
  i2s_set_pin(I2S_NUM_0, &broches);
  xTaskCreatePinnedToCore(tacheSon, "son", 4096, nullptr, 2, nullptr, 0);
}

void sonJouer(uint16_t frequence, uint16_t dureeMs) {
  if (frequence == 0) { sonArreter(); return; }
  finNoteMs = dureeMs ? millis() + dureeMs : 0;
  // Pas sur 32 bits : la table est lue avec les 8 bits de poids fort de la phase.
  pasPhase = (uint32_t)((double)frequence * 4294967296.0 / FREQ_ECHANTILLONNAGE);
}

void sonArreter() {
  pasPhase = 0;
  finNoteMs = 0;
}

void sonMelodie(const uint16_t* notes, const uint16_t* durees, int nombre) {
  for (int i = 0; i < nombre; i++) {
    // La note s'arrête un peu avant sa durée pour bien détacher les notes répétées.
    if (notes[i]) sonJouer(notes[i], durees[i] * 85 / 100);
    else sonArreter();
    delay(durees[i]);
  }
  sonArreter();
}
