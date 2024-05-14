#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "errorHandlers.h"
#include "uart_ctrl.h"

UART_HandleTypeDef uartPeriphHandler;


/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Initializes UART with predefined parameters:
                      Baud rate - 115200
                      Wordlength - 8bit
                      Stopbits - 1
                      Parity - None
*/
/* ----------------------------------------------------------------------------
 */
void uartInit(void)
{
    uartPeriphHandler.Instance = USART1;
    uartPeriphHandler.Init.BaudRate = 115200;
    uartPeriphHandler.Init.WordLength = UART_WORDLENGTH_8B;
    uartPeriphHandler.Init.StopBits = UART_STOPBITS_1;
    uartPeriphHandler.Init.Parity = UART_PARITY_NONE;
    uartPeriphHandler.Init.Mode = UART_MODE_TX_RX;
    uartPeriphHandler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartPeriphHandler.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&uartPeriphHandler) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Transmit data via UART in polling blocking mode

 @param[in]     txData pointer to a buffer to transmit data from.
 @param[in]     size transmit data size.
 @param[in]     timeout Operation timeout.

 @return        Status of operation. 
*/
/* ----------------------------------------------------------------------------
 */
HAL_StatusTypeDef uartTransmit(uint8_t *txData, uint16_t size, uint32_t timeout)
{
    HAL_StatusTypeDef ret = HAL_UART_Transmit(&uartPeriphHandler, txData, size, timeout);
    if(ret != HAL_OK && ret != HAL_TIMEOUT)
    {
        Error_Handler();
    }
    return ret;
}

/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Receive data via UART in polling blocking mode

 @param[out]    rxData pointer to store received data to.
 @param[in]     size received data size.
 @param[in]     timeout Operation timeout.

 @return        Status of operation. 
*/
/* ----------------------------------------------------------------------------
 */
HAL_StatusTypeDef uartReceive(uint8_t *rxData, uint16_t size, uint32_t timeout)
{
    HAL_StatusTypeDef ret = HAL_UART_Receive(&uartPeriphHandler, rxData, size,timeout);
    if(ret != HAL_OK && ret != HAL_TIMEOUT)
    {
        Error_Handler();
    }
    return ret;
}

/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Send one byte of data via UART in polling blocking mode

 @param[in]     timeout Operation timeout.

 @return        EOF if error, sent byte otherwise
*/
/* ----------------------------------------------------------------------------
 */
int32_t uartSendChar(char c)
{
    if(HAL_UART_Transmit(&uartPeriphHandler, (uint8_t*)&c, 1, 2) != HAL_OK)
    {
        return EOF;
    }
    return c;
}

/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Receive one byte of data via UART in polling blocking mode

 @return        EOF if error, received byte otherwise
*/
/* ----------------------------------------------------------------------------
 */
int32_t uartGetChar(void)
{
    int32_t c = EOF;
    if(__HAL_UART_GET_FLAG(&uartPeriphHandler, UART_FLAG_RXNE) != HAL_OK)
    {
        c = (uint8_t)(READ_BIT(uartPeriphHandler.Instance->DR, 0x1FF) & 0xFF);
    }
    return c;
}