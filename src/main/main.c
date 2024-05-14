#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "board.h"
#include "uart_ctrl.h"
#include "gpio_ctrl.h"
#include "rcc_ctrl.h"
#include "timer_ctrl.h"
#include "retarget.h"
#include "memcfg.h"

static void ledTimerFunc(void);

static timerHandler ledTimer =
{
    .periodMs = 1000, // 1 second period
    .counter  = 1000,
    .callback = ledTimerFunc
};
    
static void ledTimerFunc(void)
{
    HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
}

void checkRunApp(void)
{
    printf("Check application before run\n");
    // CHeck whether pushbutton is pressed
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
        printf("Staring application\n");
        volatile uint32_t mspVal = FLASH_APP_START;
        HAL_RCC_DeInit();
        HAL_DeInit();
        __set_MSP(mspVal);
        SysTick->CTRL = 0;
        SysTick->LOAD = 0;
        SysTick->VAL = 0;

        fPointer jumpFunc = (void*)(*((volatile uint32_t *)(FLASH_APP_START + 4)));
        SCB->VTOR = FLASH_APP_START;
        jumpFunc();
    }
    else 
    {
        printf("Button pressed, staying in boot\n");
        return;
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    gpioInit();
    boardInitPins();
    retargetInit();
    uartInit();
    timerInit();
    printf("Bootloader\n");
    printf("Build time: %s %s\n", __DATE__, __TIME__);
    checkRunApp(); 
    timerAdd(&ledTimer);
    while (1)
    {
        timerPerformCheck();
    }
}
