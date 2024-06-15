#include <stdio.h>
#include "power.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "Active Core";
static const BaseType_t app_cpu = 0;

static TaskHandle_t task_handle_1 = NULL;
static TaskHandle_t task_handle_2 = NULL;

void task_1(void *parameters)
{
    // esp_rom_gpio_pad_select_gpio(GPIO_NUM_2);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    while (1)
    {
        ESP_LOGI("Task 1", "running on Core %d", xPortGetCoreID());
        gpio_set_level(GPIO_NUM_2, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS); // This has to exist to relinquish control back to the scheduler.
        gpio_set_level(GPIO_NUM_2, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void task_2()
{

    for (;;)
    {
        ESP_LOGI("Task 2", "running on Core %d", xPortGetCoreID());
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void power_on(void)
{
    ESP_LOGI("power_on", "Booting");

    xTaskCreatePinnedToCore(task_1, "Task1", 2048, NULL, 1, &task_handle_1, app_cpu);
    xTaskCreatePinnedToCore(task_2, "Task2", 2048, NULL, 1, &task_handle_2, app_cpu);
    //xTaskCreate(task_1, "Task1", 2048, NULL, 1, &task_handle_1);
    //xTaskCreate(task_2, "Task2", 2048, NULL, 1, &task_handle_2);
}