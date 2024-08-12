//declara as funções e estruturas usadas para a comunicação LoRa

#ifndef LORA1914_H
#define LORA1914_H

#include "Arduino.h"
#include "LoRa.h"
#include "string.h"

//Estrutura para armazenar as configurações do módulo LoRa
typedef struct 
{
    uint8_t ss;
    uint8_t rst;
    uint8_t dio0;
    double frequency;
}Data;

//Estrutura para armazenar os identificadores dos dispositivos
typedef struct 
{
    char ID_Gateway[17];
    char ID_Master[13];
    uint32_t *ID_Slave;
}ID_Data;

//Enumeração para definir os estados de transmissão e recepção
enum
{
    TX_COMPLETE, //Transmissão completa
    RX_COMPLETE, //Recepção completa
    RX_TIMEOUT, //Timeout na recepção
    TX_TIMEOUT, //Timeout na transmissão
};

void LoRa_begin(Data *data, ID_Data *id_data); //Inicializa o módulo LoRa com as configurações especificadas

uint8_t LoRa_Send(ID_Data *id_data, char *payload); //Envia um payload via LoRa

uint32_t LoRa_Receive(ID_Data *id_data, String *payload); //Recebe um payload via LoRa

void LoRa_RecieveCallBack(uint32_t *ack); //Envia um ack de confirmação para a outra parte na comunicação



#endif // LORA1914_H
