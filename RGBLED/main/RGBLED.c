#include "ConfigLED.h"

void app_main(void) {
	ledc_init();
	configAnalogicInput();

	for(;;){
		analogicColor();
	}
}