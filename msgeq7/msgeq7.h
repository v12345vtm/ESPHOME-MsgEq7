#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace msgeq7 {

class MSGEQ7Component : public PollingComponent {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_strobe_pin(GPIOPin *pin) { strobe_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { reset_pin_ = pin; }
  void set_analog_pin(uint8_t pin) { analog_pin_ = pin; }
  
  void set_sensor(uint8_t index, sensor::Sensor *sensor) { 
      if (index < 7) sensors_[index] = sensor; 
  }

 protected:
  GPIOPin *strobe_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  uint8_t analog_pin_;
  sensor::Sensor *sensors_[7] = {nullptr};
};

}  // namespace msgeq7
}  // namespace esphome