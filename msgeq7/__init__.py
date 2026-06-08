import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import CONF_ID

msgeq7_ns = cg.esphome_ns.namespace("msgeq7")
MSGEQ7Component = msgeq7_ns.class_("MSGEQ7Component", cg.PollingComponent)

CONF_STROBE_PIN = "strobe_pin"
CONF_RESET_PIN = "reset_pin"
CONF_ANALOG_PIN = "analog_pin"

# We use a rapid polling interval for audio reactivity, default 50ms
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MSGEQ7Component),
    cv.Required(CONF_STROBE_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_RESET_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_ANALOG_PIN): pins.internal_gpio_input_pin_schema,
}).extend(cv.polling_component_schema("50ms"))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    strobe = await cg.gpio_pin_expression(config[CONF_STROBE_PIN])
    cg.add(var.set_strobe_pin(strobe))
    
    reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
    cg.add(var.set_reset_pin(reset))
    
    # Pass the raw analog pin number for fast analogRead() loop
    cg.add(var.set_analog_pin(config[CONF_ANALOG_PIN]["number"]))