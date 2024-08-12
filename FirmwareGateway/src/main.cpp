#include <Arduino.h>
#include <loRa1914.h>
#include <freertos/queue.h>
#include <task_lora.h>
#include <task_mqtt.h>


void setup() {
Serial.begin(115200);
  Serial.println("Starting...");

  //cria uma fila com capacidade para 10 elementos do tipo String
  QueueHandle_t queue = xQueueCreate(10 , sizeof(String));
  if(queue == NULL)
  {
    Serial.println("Queue not created");
    return;
  }
  //Define o tamanho da pilha para as tarefas
  size_t stackSize = 65536 + sizeof(String);

  //cria duas tarefas e as coloca nos núcleos 0 e 1
  xTaskCreatePinnedToCore(task_lora, "task_lora", stackSize, (void *)queue, 1, NULL, 0);
  xTaskCreatePinnedToCore(task_mqtt, "task_mqtt", stackSize, (void *)queue, 1, NULL, 1);
}

void loop() {}

