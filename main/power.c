#include <stdio.h>
#include "power.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "power";
static const BaseType_t app_cpu = 1;

static TaskHandle_t task_handle_1 = NULL;


void task_1(void *parameters){

    /*
    while(1){
        ESP_LOGI(TAG, "Task 1 is running");
        vTaskDelay(20/portTICK_PERIOD_MS);
    }*/
   vTaskDelay(100/portTICK_PERIOD_MS);
}

void power_on(void) {
    ESP_LOGI(TAG, "Booting");

    xTaskCreatePinnedToCore(task_1, "Task1", 1024, NULL, 2, &task_handle_1, app_cpu);

    vTaskDelay(100/portTICK_PERIOD_MS);
    

    
    if( task_handle_1 != NULL )
    {
        vTaskDelete( task_handle_1 );
    }
    
}