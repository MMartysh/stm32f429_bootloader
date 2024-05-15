#include "inc/retarget.h"
#include <stdio.h>
#include <stdbool.h>
#include "unistd.h"
#include "stm32f4xx_hal.h"
#include "uart_ctrl.h"

FILE __stdout;
FILE __stdin;
FILE __stderr;

void retargetInit(void)
{
  setvbuf(&__stdin, NULL, _IONBF, 0);
  setvbuf(&__stdout, NULL, _IONBF, 0);
  setvbuf(&__stderr, NULL, _IONBF, 0);
}

int getchar(void)
{
  return uartGetChar();
}

ssize_t _write(int file, char *ptr, size_t len)
{
  if(file == STDOUT_FILENO)
  {
    for(int i = 0; i < len; i++)
    {
      if (ptr[i] == '\n')
      {
        (void)uartSendChar('\r');
      }
      uartSendChar((char)ptr[i]);
    }
  }
  else 
  {
    return -1;
  }
    return len;
}
