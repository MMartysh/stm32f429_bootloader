#ifndef BOARD_H
#define BOARD_H

typedef struct gpioPinCfg
{
  GPIO_TypeDef *gpioPortHandle;
  GPIO_InitTypeDef gpioPinHandle;
}gpioPinCfg;

void boardInitPins(void);

#endif
