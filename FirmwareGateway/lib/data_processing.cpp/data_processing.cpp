#include "data_processing.h"

// id_data 
void dataProcessing(data_sensor_t *sensor, ID_Data *id_data, char **p_dados)
{
    // Calcula o tamanho necessário da string formatada
    int tamanho_string = snprintf(NULL, 0, "{\"tx\": \"%s\", \"rx\": \"%s\", \"data\": \"%03d%03d%d%08.6f%08.6f\"}", 
                                  id_data->ID_Master, id_data->ID_Gateway, 
                                  sensor->volume, sensor->bateria, sensor->status, 
                                  sensor->latitude, sensor->longitude);
    
    // Aloca memória para a string formatada
    *p_dados = (char*)malloc((tamanho_string + 1) * sizeof(char));

    if(*p_dados != NULL)
    {
        // Formata a string com os dados do sensor e IDs
        sprintf(*p_dados, "{\"tx\": \"%s\", \"rx\": \"%s\", \"data\": \"%03d%03d%d%08.6f%08.6f\"}", 
                id_data->ID_Master, id_data->ID_Gateway, 
                sensor->volume, sensor->bateria, sensor->status, 
                sensor->latitude, sensor->longitude);
        
        // Imprime a string formatada para depuração
        printf("CORE 1: Data: %s\n", *p_dados);
    }
    else
    {
        // Lida com erro de alocação de memória
        Serial.println("Erro ao alocar memoria");
        delay(1000);
        ESP.restart();
    }
}
