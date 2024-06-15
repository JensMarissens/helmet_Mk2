#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "power.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/touch_pad.h"

// static const char *TAG = "Active Core";
static const BaseType_t app_cpu = 0;

static TaskHandle_t task_handle_1 = NULL;
static TaskHandle_t task_handle_2 = NULL;
static TaskHandle_t task_handle_3 = NULL;

void task_1(void *parameters)
{
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    while (1)
    {
        ESP_LOGI("Task 1", "running on Core %d", xPortGetCoreID());

        gpio_set_level(GPIO_NUM_2, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // This has to exist to relinquish control back to the scheduler.
        gpio_set_level(GPIO_NUM_2, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        uint32_t watermark = uxTaskGetStackHighWaterMark(task_handle_1);
        ESP_LOGW("Memory", "Task 1 Stack High Watermark: %u bytes", (int)watermark * sizeof(StackType_t));
    }
}

void task_2(void *parameters)
{
    unsigned int seed_value = (unsigned int)time(NULL);
    srandom(seed_value);

    for (;;)
    {
        ESP_LOGI("Task 2", "running on Core %d", xPortGetCoreID());

        unsigned int rand1 = random() % 999;
        unsigned int rand2 = random() % 999;
        unsigned int rand = rand1 * rand2;

        uint32_t watermark = uxTaskGetStackHighWaterMark(task_handle_2);
        ESP_LOGW("Memory", "Task 2 Stack High Watermark: %u bytes", (int)watermark * sizeof(StackType_t));
        ESP_LOGI("Task 2", "Random: %u", rand);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_3(void *parameters)
{
    esp_err_t err = touch_pad_init();
    if (err != ESP_OK)
    {
        ESP_LOGE("Task 3", "Touch pad initialization failed: %s", esp_err_to_name(err));
        return;
    }

    // Configure the touch pad GPIO
    err = touch_pad_config(TOUCH_PAD_NUM4, 400);
    if (err != ESP_OK)
    {
        ESP_LOGE("Task 3", "Touch pad configuration failed: %s", esp_err_to_name(err));
        return;
    }

    // Set up the touch pad filter
    touch_pad_filter_start(10);

    for (;;)
    {
        uint16_t touch_value;
        // Read the raw value from the touch pad
        err = touch_pad_read(TOUCH_PAD_NUM4, &touch_value);
        if (err == ESP_OK)
        {
            bool is_touched = (touch_value < 400);  // Adjust threshold comparison

            if (is_touched)
            {
                ESP_LOGI("Touch Sensor", "Touch detected!");
                
            }
            //ESP_LOGI("Task 3", "Touch pad value: %d", touch_value);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        else
        {
            ESP_LOGE("Task 3", "Failed to read touch pad value: %s", esp_err_to_name(err));
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1000ms (1 second)

        uint32_t watermark = uxTaskGetStackHighWaterMark(task_handle_3);
        ESP_LOGW("Memory", "Task 3 Stack High Watermark: %u bytes", (int)watermark * sizeof(StackType_t));
    }
}

void power_on(void)
{
    ESP_LOGI("power_on", "Booting");

    xTaskCreatePinnedToCore(task_1, "Task1", 2048, NULL, 1, &task_handle_1, 1);
    xTaskCreatePinnedToCore(task_2, "Task2", 2048, NULL, 2, &task_handle_2, app_cpu);
    xTaskCreatePinnedToCore(task_3, "Task3", 4096, NULL, 4, &task_handle_3, app_cpu);

    for (;;)
    {
        vTaskResume(task_handle_3);
        ESP_LOGI("Task 3", "RESUME");
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        ESP_LOGI("Task 3", "SUSPEND");
        vTaskSuspend(task_handle_3);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}