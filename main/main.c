#include <stdio.h>
#include "power.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// power modes and management
// alphanumericToBitmap library
// display animations

void app_main(void)
{
    power_on();
    vTaskDelay(100/portTICK_PERIOD_MS);
}