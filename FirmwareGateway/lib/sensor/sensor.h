#ifndef SENSOR_H
#define SENSOR_H

#include "Arduino.h"
#include "loRa1914.h"
#include "shifterRegister.h"
#include "Adafruit_TSL2561_U.h"
#include "defines.h"
#include "stdint.h"

typedef struct 
{
    uint32_t volume;
    uint8_t bateria;
    uint8_t status;
    double latitude;
    double longitude;
};

void sensors_init(Adafruit_TSL2561_Unified *tsl, ShifterRegister *shifter);
void getValues(data_sensor_t *sensor, Adafruit_TSL2561_Unified *tsl, ShifterRegister *shifter);
void sumValues(data_sensor_t *sensor, uint32_t cont);

#endif