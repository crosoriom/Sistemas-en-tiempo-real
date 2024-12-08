#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define SPEED_MODE LEDC_LOW_SPEED_MODE
#define GPIO_LED_RED 4
#define GPIO_LED_GREEN 16
#define GPIO_LED_BLUE 17
#define PWM_FREC 4000

static void ledc_init(void) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = PWM_FREC,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .clk_cfg          = LEDC_APB_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel_r = {
        .speed_mode     = SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_LED_RED,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config_t ledc_channel_g = {
        .speed_mode     = SPEED_MODE,
        .channel        = LEDC_CHANNEL_1,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_LED_GREEN,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config_t ledc_channel_b = {
        .speed_mode     = SPEED_MODE,
        .channel        = LEDC_CHANNEL_2,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = GPIO_LED_BLUE,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel_r);
    
    ledc_channel_config(&ledc_channel_g);
    ledc_channel_config(&ledc_channel_b);
}

void configLedColor(uint8_t r, uint8_t g, uint8_t b) {
    ledc_set_duty(SPEED_MODE, LEDC_CHANNEL_0, r);
    ledc_set_duty(SPEED_MODE, LEDC_CHANNEL_1, g);
    ledc_set_duty(SPEED_MODE, LEDC_CHANNEL_2, b);

    ledc_update_duty(SPEED_MODE, LEDC_CHANNEL_0);
    ledc_update_duty(SPEED_MODE, LEDC_CHANNEL_1);
    ledc_update_duty(SPEED_MODE, LEDC_CHANNEL_2);
}

void colorLoop(void) {
    for(int i = 0; i <= 255; i++){
        for (int j = 0; j <= 255; j++) {
            configLedColor(i, j , j - i);
            vTaskDelay(1);
        }
    }
}