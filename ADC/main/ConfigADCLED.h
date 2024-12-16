#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define SPEED_MODE LEDC_LOW_SPEED_MODE
#define GPIO_LED_RED 4
#define GPIO_LED_GREEN 16
#define GPIO_LED_BLUE 17
#define PWM_FREC 4000

#define ANALOG_INPUT ADC_CHANNEL_7

#define BUTTON 0

typedef enum {
	COLOR = 0,
	RED,
	GREEN,
	BLUE
}state_t;

adc_oneshot_unit_handle_t adc_handle = NULL;

static void ledc_init(void) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = PWM_FREC,
        .duty_resolution  = LEDC_TIMER_10_BIT,
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

void configGpioInput(void) {
    gpio_config_t in_conf = {};
    in_conf.pin_bit_mask = BUTTON;
    in_conf.mode = GPIO_MODE_INPUT;
    in_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    in_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    in_conf.intr_type = GPIO_INTR_POSEDGE;
    gpio_config(&in_conf);
}

void configAnalogicInput(void) {
    adc_oneshot_unit_init_cfg_t adc_init = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    adc_oneshot_new_unit(&adc_init, &adc_handle);

    adc_oneshot_chan_cfg_t adc_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_10
    };
    adc_oneshot_config_channel(adc_handle, ANALOG_INPUT, &adc_config);
}

void configLedColor(uint8_t color, int channel) {
    ledc_set_duty(SPEED_MODE, channel, color);
    ledc_update_duty(SPEED_MODE, channel);
}

void redTask(void* stage_ptr) {
	state_t* stage = (state_t*)stage_ptr;
	int red;
	for(;;) {
		if(*stage == RED) {
    		adc_oneshot_read(adc_handle, ANALOG_INPUT, &red);
    		red = (uint8_t)(red * 255 / 1023);
    		configLedColor(red, LEDC_CHANNEL_0);
		}
        vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void greenTask(void* stage_ptr) {
	state_t* stage = (state_t*)stage_ptr;
	int green;
	for(;;) {
		if(*stage == GREEN) {
    		adc_oneshot_read(adc_handle, ANALOG_INPUT, &green);
    		green = (uint8_t)(green * 255 / 1023);
    		configLedColor(green, LEDC_CHANNEL_1);
		}
        vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void blueTask(void* stage_ptr) {
	state_t* stage = (state_t*)stage_ptr;
	int blue;
	for(;;) {
		if(*stage == BLUE) {
    		adc_oneshot_read(adc_handle, ANALOG_INPUT, &blue);
    		blue = (uint8_t)(blue * 255 / 1023);
    		configLedColor(blue, LEDC_CHANNEL_2);
		}
        vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void buttonTask(void* stage_ptr) {
	state_t* stage = (state_t*)stage_ptr;
    for(;;) {
        if(gpio_get_level(BUTTON) == 0) {
            (*stage)++; // Incrementar el valor del estado
            if (*stage > BLUE)
                *stage = COLOR; // Resetear el estado si pasa de BLUE
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}