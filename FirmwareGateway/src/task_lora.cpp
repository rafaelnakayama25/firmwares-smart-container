#include "task_lora.h"
#include "freertos/queue.h"

typedef enum {
    LORA_SEND,
    LORA_RECEIVE,
    VERIFY_QUEUE,
    VERIFY_RECEIVE,
    DATA_PROCESSING,
} machine_state_t;



void initializeLoraControl(ID_Data* id_data, Data* data, ShifterRegister* shifter) 
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

    shifter->begin();
    shifter->controlPorts(0x00, MSBFIRST);
    LoRa_begin(data, id_data);

}

void handleVerifyReceive(String *received, ID_Data *id_data, Machine_state_t *state)
{
    char *id_tx_str = parseToString("tx", received->c_str());
    if(id_tx_str != NULL)
    {
        Serial.printf("data to verify: %s", id_tx_str);
        if (id_tx_str && strcmp(id_tx_str, id_data->ID_Gateway) == 0)
        {
            
            *state = RELE_CONTROL;
            return;
        }  
    }
    
        received->clear();
        *state = VERIFY_QUEUE;
}

void handleVerifyQueue(QueueHandle_t *queue, Sensor_data_t *sensor, Machine_state_t *state)
{
    if (xQueueReceive(*queue, sensor, pdMS_TO_TICKS(10)) == pdPASS)
        *state = DATA_PROCESSING;
    else 
        *state = LORA_RECEIVE;
}


void task_lora_control(void *pvParameters)
{
    QueueHandle_t queue = (QueueHandle_t)pvParameters;
    Sensor_data_t sensor;
    ID_Data id_data;
    Data data;
    String received = "";
    char *p_dados = nullptr;
    Machine_state_t state = LORA_RECEIVE;
    ShifterRegister shifter(CLOCK_SHIFTER, LATCH_SHIFTER, DATA_SHIFTER);


    initializeLoraControl(&id_data, &data, &shifter);
    while(1)
    {
        switch(state)
        {
            case LORA_RECEIVE:
                LoRa_Receive(&id_data, &received);

                if(!received.isEmpty())
                {
                    Serial.println(received);
                    state = VERIFY_RECIVE;
                    break;
                }
                state = VERIFY_QUEUE;
                break;

            case VERIFY_RECIVE:
                handleVerifyReceive(&received, &id_data, &state);
                break;

            case RELE_CONTROL:
                releControl(&id_data, &shifter, received);
                received.clear();
                state = LORA_RECEIVE;
                break;

            case VERIFY_QUEUE:
                handleVerifyQueue(&queue, &sensor, &state);
                break;

            case DATA_PROCESSING:
                dataProcessing(&sensor.data_sensor, &sensor.data_liquid, &id_data, &p_dados);
                state = LORA_SEND;
                break;

            case LORA_SEND:
                if(LoRa_Send(&id_data, p_dados) == TX_COMPLETE)
                {
                    Serial.println("TX_COMPLETE");
                    LoRa.receive();
                    state = LORA_RECEIVE;
                    break;
                }
                Serial.println("TX_TIMEOUT");
                break;

            default:
                state = LORA_RECEIVE;
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}