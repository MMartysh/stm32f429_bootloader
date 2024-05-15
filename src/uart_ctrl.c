#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "errorHandlers.h"
#include "uart_ctrl.h"
#include "memcfg.h"
#include "terminal.h"

#define UPD_TIMEOUT     1000

UART_HandleTypeDef uartPeriphHandler;
static bool isUpdInitiated = false;
static uint32_t imageSize;
static uint32_t imageCrc;
typedef union updBuff{
    uint32_t u32;
    uint8_t  u8[4];
}updBuff;
bool getUpdStatus(void)
{
    return isUpdInitiated;
}
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

/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Handler function for update terminal command

 @param[in]     argc nummber of arguments
 @param[in]     argv arguments array

 @return        Status of operation
*/
/* ----------------------------------------------------------------------------
 */
bool terminalUpd(uint8_t argc, char **argv)
{
    if(!terminalGetInt(argv[1], &imageSize))
    {
        return false;
    }
    if(imageSize > FLASH_APP_SIZE)
    {
        return false;
    }
    isUpdInitiated = true;
    __HAL_UART_FLUSH_DRREGISTER(&uartPeriphHandler);
    __HAL_UART_CLEAR_FLAG(&uartPeriphHandler, UART_FLAG_RXNE);
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
    FLASH_Erase_Sector(FLASH_SECTOR_4, VOLTAGE_RANGE_3);
    return true;
}

void receivePacket(void)
{
    static uint32_t receivedSize = 0;
    static uint32_t flashOffset = 0; 
    int32_t recChar;
    static updBuff data;
    if((recChar = getchar()) == EOF)
    {
        return;
    }
    data.u8[receivedSize++] = (char)recChar;
    if(receivedSize == sizeof(data))
    {
        receivedSize = 0;
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_APP_START + flashOffset, data.u32) != HAL_OK)
        {
            isUpdInitiated = false;
            flashOffset = 0;
            // Send ERROR code
            uartSendChar(0xEB);
            return;
        }   
    }

    flashOffset += sizeof(data);
    if(flashOffset >= imageSize)
    {
        isUpdInitiated = false;
        flashOffset = 0;
        HAL_FLASH_Lock();
    }
    // Send acknowledge-
    uartSendChar(0x79);
}

/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Add terminal command handle to list
*/
/* ----------------------------------------------------------------------------
 */
__attribute__((constructor))
void terminalUpdInit(void)
{
    static commandStruct updCommand = 
    { 
        .name = "update",
        .description = "Initiate application update. \n\n  update [arg1][arg2]"
                     "arg1 - binary size"
                     "arg2 - binary CRC",
        .callback = terminalUpd,
        .next = NULL 
    };
    terminalAddCommand(&updCommand);
}
