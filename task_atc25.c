#include "FreeRTOS.h"
#include "task.h"
#include "systick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "atc_e25.h"


extern uint8_t led_mode;
extern uint8_t timer_mode;
extern uint8_t alarm;
extern uint8_t counter;

void vTaskProcessing(void *pvParameters) {
    (void) pvParameters;
    while (1) {
        atc25_led_on(ALL); // Turn on all LEDs to indicate the task is running
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
        atc25_led_off(ALL); // Turn off all LEDs
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // Handle stack overflow here
    atc25_led_flash(ALL, 200);
    for (;;) {
        // Infinite loop to indicate stack overflow
    }
}