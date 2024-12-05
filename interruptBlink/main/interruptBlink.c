#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define BUTTON_GPIO 0
#define BLINK_GPIO 2
#define BLINK_PERIOD_MS 500

typedef enum {
    INACTIVE,
    ACTIVE
}state_t;

TaskHandle_t ledTaskHandle;

void ledTask(void* arg) {
    const TickType_t ledDelay = BLINK_PERIOD_MS / portTICK_PERIOD_MS;
    uint8_t ledState = arg;

    for(;;) {
        gpio_set_level(BLINK_GPIO, ledState);
        ledState = !ledState;
        vTaskDelay(ledDelay);
    }
}

void buttonTask(void* arg) {
    state_t ledState = ACTIVE;
    for(;;) {
        if(gpio_get_level(BUTTON_GPIO) == 0) {
            ledState = !ledState;
            if(ledState == ACTIVE) {
                vTaskSuspend(ledTaskHandle);
            } else {
                vTaskResume(ledTaskHandle);
            }
            vTaskDelay(100);
        }
    }
}

static void configureLed(void) {
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

static void configureButton(void) {
    gpio_config_t in_conf = {};
    in_conf.pin_bit_mask = BUTTON_GPIO;
    in_conf.mode = GPIO_MODE_INPUT;
    in_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    in_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    in_conf.intr_type = GPIO_INTR_POSEDGE;
    gpio_config(&in_conf);
}

void app_main(void) {
    configureLed();
    configureButton();
    state_t ledState = INACTIVE;

    xTaskCreate(ledTask, "Blinking", 1024, &ledState, 1, &ledTaskHandle);
    xTaskCreate(buttonTask, "Interruption", 1024, NULL, 1, NULL);
}
