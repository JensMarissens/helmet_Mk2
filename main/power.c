#include <stdio.h>
#include "power.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "LED_blink";
static const BaseType_t app_cpu = 0;

static TaskHandle_t task_handle_1 = NULL;
static TaskHandle_t task_handle_2 = NULL;

void task_1(void *parameters)
{
    // esp_rom_gpio_pad_select_gpio(GPIO_NUM_2);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    while (1)
    {
        ESP_LOGI(TAG, "Task 1 is running");
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
        ESP_LOGI("Multitasking", "Task 2 is running");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void power_on(void)
{
    ESP_LOGI(TAG, "Booting");

    xTaskCreatePinnedToCore(task_1, "Task1", 2048, NULL, 1, &task_handle_1, app_cpu);
    xTaskCreatePinnedToCore(task_2, "Task2", 2048, NULL, 1, &task_handle_2, app_cpu);
}