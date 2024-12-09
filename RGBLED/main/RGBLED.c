#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "ConfigLED.h"

void app_main(void) {
	ledc_init();
	configAnalogicInput();

	for(;;){
		analogicColor();
	}
}