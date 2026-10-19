#include "micro.h"
#include "reglages.h"
#include <driver/i2s.h>
#include <math.h>

static const i2s_port_t PORT = I2S_NUM_1;
static float moyenne = 0;  // suit la composante continue du micro pour la retirer

void microDemarrer(uint8_t brocheSck, uint8_t brocheWs, uint8_t brocheSd) {
  i2s_config_t config = {};
  config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX);
  config.sample_rate = MICRO_FREQUENCE;
  // L'INMP441 envoie 24 bits utiles dans un mot de 32 bits.
  config.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
  config.channel_format = MICRO_CANAL_DROIT ? I2S_CHANNEL_FMT_ONLY_RIGHT : I2S_CHANNEL_FMT_ONLY_LEFT;
  config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  config.dma_buf_count = 4;
  config.dma_buf_len = 256;

  i2s_pin_config_t broches = {};
  broches.mck_io_num = I2S_PIN_NO_CHANGE;
  broches.bck_io_num = brocheSck;
  broches.ws_io_num = brocheWs;
  broches.data_out_num = I2S_PIN_NO_CHANGE;
  broches.data_in_num = brocheSd;

  i2s_driver_install(PORT, &config, 0, nullptr);
  i2s_set_pin(PORT, &broches);
  i2s_stop(PORT);
}

void microActiver(bool actif) {
  if (actif) {
    i2s_zero_dma_buffer(PORT);
    i2s_start(PORT);
    // Le micro met quelques dizaines de millisecondes à se stabiliser.
    int16_t poubelle[256];
    for (int i = 0; i < 4; i++) microLire(poubelle, 256);
  } else {
    i2s_stop(PORT);
  }
}

size_t microLire(int16_t* destination, size_t nombre) {
  int32_t brut[128];
  size_t total = 0;
  while (total < nombre) {
    size_t aLire = min(nombre - total, (size_t)128);
    size_t octetsLus = 0;
    i2s_read(PORT, brut, aLire * sizeof(int32_t), &octetsLus, portMAX_DELAY);
    size_t lus = octetsLus / sizeof(int32_t);
    for (size_t i = 0; i < lus; i++) {
      float e = (float)(brut[i] >> 14);
      moyenne += (e - moyenne) * 0.001f;
      e = (e - moyenne) * MICRO_GAIN;
      destination[total + i] = (int16_t)constrain(e, -32767.0f, 32767.0f);
    }
    total += lus;
  }
  return total;
}

int microNiveau() {
  int16_t bloc[256];
  microLire(bloc, 256);
  float somme = 0;
  for (int i = 0; i < 256; i++) somme += (float)bloc[i] * bloc[i];
  float efficace = sqrtf(somme / 256);
  // Échelle logarithmique : 40 dB de dynamique utile, de la voix douce au clap.
  float db = 20 * log10f(max(efficace, 1.0f));
  return constrain((int)((db - 40) * 100 / 40), 0, 100);
}
