#include "msgeq7.h"
#include "esphome/core/log.h"

#ifdef USE_ARDUINO
#include <Arduino.h>
#endif

#ifdef USE_ESP_IDF
#include <esp_rom_sys.h>
#include <driver/adc.h>

// Helper to map GPIO34 to the native ESP-IDF ADC1 Channel
static adc1_channel_t get_adc1_channel(uint8_t pin) {
  switch(pin) {
    case 36: return ADC1_CHANNEL_0;
    case 37: return ADC1_CHANNEL_1;
    case 38: return ADC1_CHANNEL_2;
    case 39: return ADC1_CHANNEL_3;
    case 32: return ADC1_CHANNEL_4;
    case 33: return ADC1_CHANNEL_5;
    case 34: return ADC1_CHANNEL_6;
    case 35: return ADC1_CHANNEL_7;
    default: return ADC1_CHANNEL_MAX;
  }
}
#endif

namespace esphome {
namespace msgeq7 {

static const char *const TAG = "msgeq7";

void MSGEQ7Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MSGEQ7...");
  this->strobe_pin_->setup();
  this->reset_pin_->setup();
  
  this->strobe_pin_->digital_write(false);
  this->reset_pin_->digital_write(false);
  
#ifdef USE_ARDUINO
  pinMode(this->analog_pin_, INPUT);
#elif defined(USE_ESP_IDF)
  adc1_channel_t chan = get_adc1_channel(this->analog_pin_);
  if (chan != ADC1_CHANNEL_MAX) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    // Support both older and newer ESP-IDF v5 attenuation macros
    # Identifying maximum input voltage range (~3.3V)
    #ifndef ADC_ATTEN_DB_11
      adc1_config_channel_atten(chan, ADC_ATTEN_DB_12);
    #else
      adc1_config_channel_atten(chan, ADC_ATTEN_DB_11);
    #endif
  }
#endif
}

void MSGEQ7Component::update() {
  // Step 1: Reset the multiplexor
  this->reset_pin_->digital_write(true);
#ifdef USE_ARDUINO
  delayMicroseconds(1);  // tr > 100ns
#else
  esp_rom_delay_us(1);
#endif
  this->reset_pin_->digital_write(false);

#ifdef USE_ARDUINO
  delayMicroseconds(75); // trs > 72us
#else
  esp_rom_delay_us(75);
#endif

  // Step 2: Loop through all 7 frequency bands
  for (int i = 0; i < 7; i++) {
    this->strobe_pin_->digital_write(true);
#ifdef USE_ARDUINO
    delayMicroseconds(20); // ts > 18us
#else
    esp_rom_delay_us(20);
#endif
    this->strobe_pin_->digital_write(false);

#ifdef USE_ARDUINO
    delayMicroseconds(40); // to > 36us (Output settling time)
#else
    esp_rom_delay_us(40);
#endif

    // Step 3: Read the DC output
    int raw_value = 0;
#ifdef USE_ARDUINO
    raw_value = analogRead(this->analog_pin_);
#elif defined(USE_ESP_IDF)
    adc1_channel_t chan = get_adc1_channel(this->analog_pin_);
    if (chan != ADC1_CHANNEL_MAX) {
      raw_value = adc1_get_raw(chan);
    }
#endif
    
    // Convert 12-bit raw value (0-4095) to percentage (0-100%)
    float percentage = (raw_value / 4095.0f) * 100.0f;

    // Step 4: Publish to ESPHome if the sensor is defined
    if (this->sensors_[i] != nullptr) {
      this->sensors_[i]->publish_state(percentage);
    }
  }
}

void MSGEQ7Component::dump_config() {
  ESP_LOGCONFIG(TAG, "MSGEQ7 Component:");
  LOG_PIN("  Strobe Pin: ", this->strobe_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  ESP_LOGCONFIG(TAG, "  Analog Pin: %d", this->analog_pin_);
}

}  // namespace msgeq7
}  // namespace esphome