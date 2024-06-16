#include <stdio.h>

#include "peripherals.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t task_handle_1 = NULL;

void per_task()
{

    // esp_rom_gpio_pad_select_gpio(GPIO_NUM_2);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

    while (1)
    {
        ESP_LOGI("Task 1", "running on Core %d", xPortGetCoreID());

        uint32_t watermark = uxTaskGetStackHighWaterMark(task_handle_1);
        ESP_LOGW("Memory", "Task 1 Stack High Watermark: %u bytes", (int)watermark * sizeof(StackType_t));

        gpio_set_level(GPIO_NUM_2, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // This has to exist to relinquish control back to the scheduler.
        gpio_set_level(GPIO_NUM_2, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}