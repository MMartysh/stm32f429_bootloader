#ifndef TERMINAL_H
#define TERMINAL_H

#define NUMBER_OF_COMMANDS   8   /*Amount of commands*/
#define MAX_ARGUMENTS_NUMBER 20  /*Max length of command*/
#define TRANSMIT_TIMEOUT     10  /*Timeout for uart_transmit, 10ms*/
#define RECEIVE_TIMEOUT      100 /*Timeout for uart_receive,100ms*/
#define MAX_STRING_LENGTH    256 /*Max lenght of data, that can be gets*/

typedef struct commandStruct
{
  const char *name;
  const char *description;
  bool (*callback)(uint8_t argc, char **argv);
  struct commandStruct *next;
}commandStruct;

void terminalExecuteCommand(uint8_t argc, char **argv);
void terminalParse(char *buf);
void terminalGetChar(void);
void terminalInit(void);
void terminalAddCommand (commandStruct *command);
bool terminalGetInt(char *str, uint32_t *var);
#endif
