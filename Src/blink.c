#include "main.h"
#include <string.h>

// todo: do not use lwip
#include "lwip.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "lwip/ip_addr.h"
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_rtc.h>

void HAL_RTC_AlarmAEventCallback (RTC_HandleTypeDef * hrtc) {
        // WARINING: function called from alarm handler
        HAL_GPIO_TogglePin(GPIOB, LD1_Pin);
}
