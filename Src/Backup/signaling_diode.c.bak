#include "signaling_diode.h"
#include "main.h" //def
#include "cmsis_os.h"

int _frequency = 100;

void SD_TaskFunc(void* param) {
        for(;;) {
                HAL_GPIO_TogglePin(GPIOB, LD3_Pin);
                osDelay(_frequency);
        }
}

void SD_SetState(SD_DiodeState diodeState) {
        switch (diodeState)
        {
        case CONNECTING:
                _frequency = 100;
                break;
        case CONNECTED:
                _frequency = 10; // do not blink
                break;
        default:
                _frequency = 500;
        }
}