import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import STATE_CLASS_MEASUREMENT
from . import MSGEQ7Component

CONF_MSGEQ7_ID = "msgeq7_id"
CONF_BAND_63HZ = "band_63hz"
CONF_BAND_160HZ = "band_160hz"
CONF_BAND_400HZ = "band_400hz"
CONF_BAND_1KHZ = "band_1khz"
CONF_BAND_2_5KHZ = "band_2_5khz"
CONF_BAND_6_25KHZ = "band_6_25khz"
CONF_BAND_16KHZ = "band_16khz"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_MSGEQ7_ID): cv.use_id(MSGEQ7Component),
    cv.Optional(CONF_BAND_63HZ): sensor.sensor_schema(unit_of_measurement="%", accuracy_decimals=1, state_class=STATE_CLASS_MEASUREMENT),
    cv.Optional(CONF_BAND_160HZ): sensor.sensor_schema(unit_of_measurement="%", accuracy_decimals=1, state_class=STATE_CLASS_MEASUREMENT),
    cv.Optional(CONF_BAND_400HZ): sensor.sensor_schema(unit_of_measurement="%", accuracy_decimals=1, state_class=STATE_CLASS_MEASUREMENT),
    cv.Optional(CONF_BAND_1KHZ): sensor.sensor_schema(unit_of_measurement="%", accuracy_decimals=1, state_class=STATE_CLASS_MEASUREMENT),
    cv.Optional(CONF_BAND_2_5KHZ): sensor.sensor_schema(unit_of_measurement="%", accuracy_decimals=1, state_class=STATE_CLASS_MEASUREMENT),
    cv.Optional(CONF_BAND_6_25KHZ): sensor.sensor_schema(unit_of_measurement="%", accuracy_decimals=1, state_class=STATE_CLASS_MEASUREMENT),
    cv.Optional(CONF_BAND_16KHZ): sensor.sensor_schema(unit_of_measurement="%", accuracy_decimals=1, state_class=STATE_CLASS_MEASUREMENT),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_MSGEQ7_ID])
    
    bands = [
        CONF_BAND_63HZ, CONF_BAND_160HZ, CONF_BAND_400HZ, 
        CONF_BAND_1KHZ, CONF_BAND_2_5KHZ, CONF_BAND_6_25KHZ, CONF_BAND_16KHZ
    ]
    
    for index, conf_name in enumerate(bands):
        if conf_name in config:
            sens = await sensor.new_sensor(config[conf_name])
            cg.add(parent.set_sensor(index, sens))