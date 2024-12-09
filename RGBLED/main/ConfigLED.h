#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_adc/adc_oneshot.h"

#define RED_PERCENTAGE 99
#define GREEN_PERCENTAGE 50
#define BLUE_PERCENTAGE 1

#define SPEED_MODE LEDC_LOW_SPEED_MODE
#define GPIO_LED_RED 4
#define GPIO_LED_GREEN 16
#define GPIO_LED_BLUE 17
#define PWM_FREC 4000

#define BUTTON_DINAMIC_COLOR 5
#define BUTTON_STATIC_COLOR 18
#define BUTTON_STOPLIGHT 19
#define BUTTON_COLORLOOP 21

#define RED_ANALOG_INPUT ADC_CHANNEL_7
#define GREEN_ANALOG_INPUT ADC_CHANNEL_9
#define BLUE_ANALOG_INPUT ADC_CHANNEL_8

adc_oneshot_unit_handle_t adc_handle = NULL;

typedef enum{
    DinamicColor,
    StaticColor,
    StopLight,
    ColorLoop
}mode;

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

void configGpioInput() {
    gpio_config_t in_conf = {};
    in_conf.pin_bit_mask = BUTTON_DINAMIC_COLOR;
    in_conf.mode = GPIO_MODE_INPUT;
    in_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    in_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    in_conf.intr_type = GPIO_INTR_POSEDGE;
    gpio_config(&in_conf);

    in_conf.pin_bit_mask = BUTTON_STATIC_COLOR;
    gpio_config(&in_conf);

    in_conf.pin_bit_mask = BUTTON_STOPLIGHT;
    gpio_config(&in_conf);

    // in_conf.pin_bit_mask = BUTTON_COLORLOOP;
    // gpio_config(&in_conf);
}

void configAnalogicInput() {
    adc_oneshot_unit_init_cfg_t adc_init = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    adc_oneshot_new_unit(&adc_init, &adc_handle);

    adc_oneshot_chan_cfg_t adc_config = {
        .atten = ADC_ATTEN_DB_0,
        .bitwidth = ADC_BITWIDTH_10
    };
    adc_oneshot_config_channel(adc_handle, RED_ANALOG_INPUT, &adc_config);
    adc_oneshot_config_channel(adc_handle, GREEN_ANALOG_INPUT, &adc_config);
    adc_oneshot_config_channel(adc_handle, BLUE_ANALOG_INPUT, &adc_config);
}

void variableLoop() {
   for(int i = 0; i <= 255; i++){
       for (int j = 0; j <= 255; j++) {
           configLedColor(i, j , j - i);
           vTaskDelay(1);
       }
   }
}

void fixColor() {
    int r, g, b;
    r = (RED_PERCENTAGE * 255) / 100;
    g = (GREEN_PERCENTAGE * 255) / 100;
    b = (BLUE_PERCENTAGE * 255) / 100;
    configLedColor(r, g, b);
}

void stopLight() {
    configLedColor(255, 0, 0);
    vTaskDelay(1000);
    configLedColor(241, 207, 18);
    vTaskDelay(200);
    configLedColor(0, 255, 0);
    vTaskDelay(1500);
    configLedColor(241, 207, 18);
    vTaskDelay(350);
}

void analogicColor() {
    int r, g, b;
    adc_oneshot_read(adc_handle, RED_ANALOG_INPUT, &r);
    adc_oneshot_read(adc_handle, GREEN_ANALOG_INPUT, &g);
    adc_oneshot_read(adc_handle, BLUE_ANALOG_INPUT, &b);
    r = (uint8_t)(r * 255 / 1023);
    g = (uint8_t)(g * 255 / 1023);
    b = (uint8_t)(b * 255 / 1023);
    configLedColor(r, g, b);
}

// void modeTask(void* arg) {
//     mode task;
//     for(;;) {
//         if(gpio_get_level(BUTTON_DINAMIC_COLOR) != 1) {
//             task = DinamicColor;
//         }else if(gpio_get_level(BUTTON_STATIC_COLOR) != 1) {
//             task = staticColor;
//         }else if(gpio_get_level(BUTTON_STOPLIGHT) != 1) {
//             task = stopLight;
//         }else if(gpio_get_level(BUTTON_COLORLOOP) != 1) {
//             task = colorLoop;
//         }else{
//             vTaskDelay(100);
//         }
//     }
// }