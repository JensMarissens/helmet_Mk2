#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "power.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// static const char *TAG = "Active Core";
static const BaseType_t app_cpu = 0;

static TaskHandle_t task_handle_1 = NULL;
static TaskHandle_t task_handle_2 = NULL;
static TaskHandle_t task_handle_3 = NULL;

void task_1(void *parameters)
{
    // esp_rom_gpio_pad_select_gpio(GPIO_NUM_2);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    while (1)
    {
        ESP_LOGI("Task 1", "running on Core %d", xPortGetCoreID());
        gpio_set_level(GPIO_NUM_2, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // This has to exist to relinquish control back to the scheduler.
        gpio_set_level(GPIO_NUM_2, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_2()
{
    unsigned int seed_value = (unsigned int)time(NULL);
    srandom(seed_value);

    for (;;) {
        unsigned int rand1 = random() % 999;
        unsigned int rand2 = random() % 999;
        unsigned int rand = rand1 * rand2;

        ESP_LOGI("Task 2", "running on Core %d", xPortGetCoreID());
        ESP_LOGI("Task 2", "Random: %u", rand);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void task_3()
{

    for (;;)
    {
        ESP_LOGI("Task 3", "running on Core %d", xPortGetCoreID());
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void power_on(void)
{
    ESP_LOGI("power_on", "Booting");

    xTaskCreatePinnedToCore(task_1, "Task1", 2048, NULL, 1, &task_handle_1, 1);
    xTaskCreatePinnedToCore(task_2, "Task2", 2048, NULL, 2, &task_handle_2, app_cpu);
    xTaskCreatePinnedToCore(task_3, "Task3", 2048, NULL, 1, &task_handle_3, app_cpu);

    for (;;)
    {
        vTaskResume(task_handle_3);
        ESP_LOGI("Task 3", "RESUME");
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        ESP_LOGI("Task 3", "SUSPEND");
        vTaskSuspend(task_handle_3);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    // xTaskCreate(task_1, "Task1", 2048, NULL, 1, &task_handle_1);
    // xTaskCreate(task_2, "Task2", 2048, NULL, 1, &task_handle_2);
}