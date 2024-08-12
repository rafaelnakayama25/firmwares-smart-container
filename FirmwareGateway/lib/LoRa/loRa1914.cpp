#include "loRa1914.h"

//Private

//função para calcular a soma dos valores ASCII de todos os caracteres da string
unsigned long simpleChecksum(const char *str) { 
    unsigned long checksum = 0;
    while (*str) {
        checksum += (unsigned char)*str;
        str++;
    }
    return checksum;
}

//função tenta receber um pacote de dados via LoRa e verifica se o valor recebido corresponde ao ack esperado. Ela espera por um tempo máximo de 5 segundos (5000 ms).
//Utilizado para confirmar a recepção de um pacote transmitido e validar se a resposta é a esperada.
int LoRa_SendCallBack(uint32_t *ack)
{
    int packetSize;
    uint8_t tempo = 0;
    uint64_t previousMillis = millis(), currentMillis = 0;
    String received;
    uint32_t ack_verify;

    // Inicializa o modo de recebimento do LoRa
    
    packetSize = LoRa.parsePacket();
    
    while(tempo == 0)
    {
        // Coloca o módulo LoRa em modo de recebimento novamente para garantir que está pronto para receber pacotes
        packetSize = LoRa.parsePacket();
        
        if (packetSize)
        {
            while (LoRa.available())
                received += (char)LoRa.read();

            Serial.println(received);

            ack_verify = (uint32_t)strtol(received.c_str(), NULL, 10);
            if (ack_verify == *ack)
                return TX_COMPLETE;
        }

        // Atualiza currentMillis para medir o tempo de espera
        currentMillis = millis();
        if (currentMillis - previousMillis > 5000)
            tempo = 1;

        vTaskDelay(10); // Pequena espera para evitar sobrecarga da CPU
    }

    tempo = 0;
    return TX_TIMEOUT;
}


//Public

//serve para enviar um pacote de dados via LoRa 5 vezes, com um intervalo de 10 ms entre cada envio. Usado para enviar um ack de confirmação para a outra parte na comunicação.
void LoRa_RecieveCallBack(uint32_t *ack)
{
    uint8_t i;
    for(i = 0; i < 5; i++)
    {
        LoRa.beginPacket();
        LoRa.print(*ack);
        LoRa.endPacket();
        vTaskDelay(10);
    }
}

//Configura os pinos do LoRa e inicia o módulo na frequência especificada. Coloca o módulo em modo de recepção.
void LoRa_begin(Data *data, ID_Data *id_data)
{
    LoRa.setPins(data->ss, data->rst, data->dio0);
    if (!LoRa.begin(data->frequency)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    LoRa.receive();

}


//Envia um payload (5 vezes) via LoRa e calcula o checksum do payload. Depois, espera por um ack de confirmação. Transmissão de dados com verificação de recebimento.
uint8_t LoRa_Send(ID_Data *data,char *payload)
{
    uint32_t ack;
    for(int i = 0; i <= 5; i++)
    {   
        LoRa.beginPacket();
        LoRa.print(payload);
        LoRa.endPacket();
        vTaskDelay(10);
    }

    ack = simpleChecksum(payload);
    LoRa.receive();
    if(LoRa_SendCallBack(&ack) == TX_COMPLETE)
        return TX_COMPLETE;
    else
        return TX_TIMEOUT;
}

//Recebe um pacote via LoRa e verifica se ele é diferente do pacote recebido anteriormente. Se for, calcula e retorna o checksum do novo pacote. 
//Recepção de dados e verificação da integridade do pacote recebido.
uint32_t LoRa_Receive(ID_Data *id_data, String *payload)
{
    static String received_ant = "";
    bool verify = false;
    int i = 0;
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        String received = "";
        while (LoRa.available())
        {
            received += (char)LoRa.read();
        }
        *payload = received;
    }
    if(received_ant == *payload)
        return 0;

    received_ant = *payload;
    return simpleChecksum(payload->c_str());
}
