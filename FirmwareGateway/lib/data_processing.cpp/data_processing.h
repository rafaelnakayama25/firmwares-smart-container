#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include "loRa1914.h"
#include "sensor.h"

void dataProcessing(data_sensor_t *sensor, ID_Data *id_data, char **p_dados);

#endif