#include "ConfigADCLED.h"

void app_main(void) {
    ledc_init();
    configGpioInput();
    configAnalogicInput();
    state_t stage = COLOR;

    xTaskCreate(buttonTask, "Select Color", 1024, &stage, 1, NULL);
    xTaskCreate(redTask, "Set red color", 1024, &stage, 1, NULL);
    xTaskCreate(greenTask, "Set green color", 1024, &stage, 1, NULL);
    xTaskCreate(blueTask, "Set blue color", 1024, &stage, 1, NULL);
}
