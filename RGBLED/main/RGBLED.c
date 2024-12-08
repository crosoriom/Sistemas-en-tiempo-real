#include "ConfigLED.h"

void app_main(void) {
	ledc_init();
	for(;;)
		colorLoop();
}