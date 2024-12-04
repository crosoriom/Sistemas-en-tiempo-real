#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/idf_additions.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define BUTTON_GPIO 0
#define BLINK_GPIO 2
#define BLINK_PERIOD_MS 500

typedef enum {
    paused,
    active
}state;

TaskHandle_t ledTaskHandle;

void ledTask(void* arg) {
    const TickType_t ledDelay = BLINK_PERIOD_MS/ portTICK_PERIOD_MS;

    for(;;) {
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(ledDelay);
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(ledDelay);
    }
}

void buttonTask(void* arg) {
    state ledState = active;
    for(;;) {
        uint8_t buttonState = gpio_get_level(BUTTON_GPIO);
        if(buttonState == 0)
            vTaskDelay(100);
        if(buttonState == 1) {
            ledState = !ledState;
            if(ledState == active) {
                vTaskSuspend(ledTaskHandle);
            } else {
                vTaskResume(ledTaskHandle);
            }
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
    //configureButton();

    xTaskCreate(ledTask, "Blinking", 1024, &ledTaskHandle, 1, NULL);
    //xTaskCreate(buttonTask, "Interruption", 1024, NULL, 1, NULL);
}
