#ifndef UART_CTRL_H
#define UART_CTRL_H

void uartInit(void);
HAL_StatusTypeDef uartTransmit(uint8_t *p_pui_Data,uint16_t p_ui_Size, uint32_t p_ui_Timeout);
HAL_StatusTypeDef uartReceive(uint8_t *p_pui_Data,uint16_t p_ui_Size, uint32_t p_ui_Timeout);
int32_t uartSendChar(char c);
int32_t uartGetChar(void);
#endif

