#include "sensor.h"

void getValues(data_sensor_t *sensor)
{
    // Lê o estado do sensor de tombamento (0 ou 1)
    sensor->status = digitalRead(TOMBAMENTO_PIN);

    // Lê os dados do sensor VL53L0X e armazena no campo volume da estrutura sensor
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    if (measure.RangeStatus != 4) {  // Se a leitura foi bem sucedida
        sensor->volume = measure.RangeMilliMeter / 10.0;  // Converte para cm ou uma unidade apropriada
    } else {
        sensor->volume = -1;  // Valor de erro ou inválido
    }

    // Lê os dados do GPS e armazena nos campos latitude e longitude da estrutura sensor
    while (mySerial.available() > 0) {
        gps.encode(mySerial.read());
    }
    if (gps.location.isValid()) {
        sensor->latitude = gps.location.lat();
        sensor->longitude = gps.location.lng();
    } else {
        sensor->latitude = 0.0;
        sensor->longitude = 0.0;
    }

    // Lê o nível da bateria (assumindo que você tenha um método para isso)
    sensor->bateria = readBatteryLevel();  // Placeholder para a função real de leitura da bateria
}