#include "stm32f4xx_hal.h"
#include "errorHandlers.h"

/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Handles error states

 @param         None.

 @return        None. 
*/
/* ----------------------------------------------------------------------------
 */
void Error_Handler(void)
{
	while(1)
	{
        HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
        HAL_Delay(1000);
	}
}
