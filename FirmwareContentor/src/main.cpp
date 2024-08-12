#include "freertos/queue.h"
#include <Arduino.h>
#include "loRa1914.h"

// Enumeração dos estados da máquina de estados do contentor
typedef enum {
    LORA_SEND,
    LORA_RECEIVE,
    LORA_BEGIN,
    DATA_PROCESSING,
} machine_state_t;

// Inicializa o módulo LoRa e configura os identificadores do contentor e gateway
void initializeLoraControl(ID_Data* id_data, Data* data) 
{
    uint64_t mac = ESP.getEfuseMac();
    data->ss = SS;
    data->rst = RST;
    data->dio0 = DIO0;
    data->frequency = 915E6;

    strncpy(id_data->ID_Gateway, "000000001ce24b53", sizeof(id_data->ID_Gateway));
    stringToUpper(id_data->ID_Gateway);
    Serial.printf("ID Gateway: %s\n", id_data->ID_Gateway);

    snprintf(id_data->ID_Master, sizeof(id_data->ID_Master), "%04X%08X", (uint16_t)(mac >> 32), (uint32_t)mac);
    Serial.printf("ID Master: %s\n", id_data->ID_Master);

    LoRa_begin(data, id_data);
}


// Processa os dados dos sensores e prepara o payload para ser enviado
void dataProcessing(const char *data_sensor, const char *data_liquid, ID_Data *id_data, char **p_dados)
{
    // Processa os dados do sensor e prepara o payload
    snprintf(*p_dados, 100, "ID:%s, Sensor:%s, Liquid:%s", id_data->ID_Master, data_sensor, data_liquid);
}

// Função principal da task LoRa do contentor
void task_lora_control(void *pvParameters)
{
    QueueHandle_t queue = (QueueHandle_t)pvParameters;
    Sensor_data_t sensor;
    ID_Data id_data;
    Data data;
    String received = "";
    char payload[100];
    machine_state_t state = LORA_BEGIN;

    initializeLoraControl(&id_data, &data);

    while(1)
    {
        switch(state)
        {
            case LORA_BEGIN:
                LoRa_begin(&data, &id_data);
                state = LORA_SEND;
                break;

            case LORA_SEND:
                handleVerifyQueue(&queue, &sensor, &state);
                break;

            case DATA_PROCESSING:
                dataProcessing(sensor.data_sensor, sensor.data_liquid, &id_data, &payload);
                if(LoRa_Send(&id_data, payload) == TX_COMPLETE)
                {
                    Serial.println("TX_COMPLETE");
                    state = LORA_RECEIVE;
                }
                else
                {
                    Serial.println("TX_TIMEOUT");
                    state = LORA_SEND;
                }
                break;

            case LORA_RECEIVE:
                LoRa_Receive(&id_data, &received);
                if(!received.isEmpty())
                {
                    Serial.println(received);
                    // Aguardando ACK
                    state = LORA_SEND;
                }
                break;

            default:
                state = LORA_SEND;
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
