#include "msgeq7.h"
#include "esphome/core/log.h"
#include <Arduino.h>

namespace esphome {
namespace msgeq7 {

static const char *const TAG = "msgeq7";

void MSGEQ7Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MSGEQ7...");
  this->strobe_pin_->setup();
  this->reset_pin_->setup();
  
  this->strobe_pin_->digital_write(false);
  this->reset_pin_->digital_write(false);
  
  pinMode(this->analog_pin_, INPUT);
}

void MSGEQ7Component::update() {
  // Step 1: Reset the multiplexor
  this->reset_pin_->digital_write(true);
  delayMicroseconds(1);  // tr > 100ns
  this->reset_pin_->digital_write(false);
  delayMicroseconds(75); // trs > 72us

  // Step 2: Loop through all 7 frequency bands
  for (int i = 0; i < 7; i++) {
    this->strobe_pin_->digital_write(true);
    delayMicroseconds(20); // ts > 18us
    this->strobe_pin_->digital_write(false);
    delayMicroseconds(40); // to > 36us (Output settling time)

    // Step 3: Read the DC output
    int raw_value = analogRead(this->analog_pin_);
    
    // ESP32 analogRead is typically 0-4095 (12-bit). Convert to 0-100%
    float percentage = (raw_value / 4095.0f) * 100.0f;

    // Step 4: Publish to ESPHome if the sensor is defined in YAML
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