#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "stm32f4xx_hal.h"
#include "uart_ctrl.h"
#include "terminal.h"


static bool terminalHelp (uint8_t argc, char **argv);
static bool terminalEcho (uint8_t argc, char **argv);

static commandStruct echoCommand = 
{ 
        .name = "echo",
        .description = "Returns arguments given to this command\n\n  echo "
                "[arg1][arg2]...[arg n] \n\n\t arg1,arg2...arg n - arguments to return\n",
        .callback = terminalEcho,
        .next = NULL 
};
/*! Terminal item used to show list of command available and help for each ot
 * them */
static commandStruct helpCommand = 
{ 
        .name = "help",
        .description = "Provides help information for available commands\n\n  help "
                "[command] \n\n\t command - shows help for this command\n",
        .callback = terminalHelp,
        .next = &echoCommand
};

/*! Pointer to the head of the linked list of terminal items */
static commandStruct *commandListHead = &helpCommand;
/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Add command to the list

 @param[in]     command command struct to add
*/
/* ----------------------------------------------------------------------------
 */
void terminalAddCommand (commandStruct *command)
{
    //begin from the list head
    commandStruct *head = commandListHead;
    command->next = NULL;
    //if head exists
    if (head != NULL)
    { 
        //goto the last node
        while(head->next != NULL)
        {
            head = head->next;
        }
        //insert command at the end of the list
        head->next = command;
        
    }
    else
    {
        //make command a head of the list
        commandListHead = command; 
    }
}
/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Looks for terminal command in the linked list by name given

 @param[in]     name command name to search

 @return        if found, pointer to the command struct, NULL otherwise
*/
/* ----------------------------------------------------------------------------
 */
static commandStruct* terminalFindCommand (const char *name)
{
    commandStruct *currentItem = commandListHead;
    //while next node exists
    while (currentItem != NULL)
    {       
        //if name we searching is equal to current command name
        if(strcmp(name, currentItem->name) == 0)
        {
            //returning command struct that matches
            return currentItem;
        }
        //moving to the next node
        currentItem = currentItem->next;
    }
    return NULL;
}
/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Shows start message in terminal

 @param         None.

 @return        None. 
*/
/* ----------------------------------------------------------------------------
 */
void terminalInit(void)
{
    char str[] = "Terminal application v0.0.2\n";
    printf("Terminal application v0.0.2\n");
    printf("Build time: %s %s\n", __DATE__, __TIME__);
    printf("Enter \"help\" to see the list of available commands\n");
}


/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Returns arguments of the echo terminal command

 @param[in]     argc number of command arguments
 @param[in]     argv command arguments

 @return        Status of operation 
*/
/* ----------------------------------------------------------------------------
 */
static bool terminalEcho(uint8_t argc, char **argv)
{
    // Skipping first argument, because it's an echo command
    for (uint8_t argIndex = 1; argIndex != argc; argIndex++)
    {
        // printing all arguments with line feed
        printf ("%s\n", argv[argIndex]);
    }
    return true;
}

/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Shows list of available commands

 @param[in]     argc number of command arguments
 @param[in]     argv command arguments

 @return        Status of operation 
*/
/* ----------------------------------------------------------------------------
 */
static bool terminalHelp(uint8_t argc, char **argv)
{
    commandStruct *operation;
    // if argv contains command without arguments
    if (argc == 1)
    {
        //begin from the list head
        operation = commandListHead;
        // printing short description for all operations
        while (operation != NULL)
        {
            printf ("%s: %s \n\n",operation->name, operation->description);
            operation = operation->next;
        }
    }
    else
    {
        // trying to find operation, given as argument
        operation = terminalFindCommand (argv[1]);
        // if operation wasn't found
        if (operation == NULL)
        {
            // returning error message
            printf ("Command was not found\n");
            return false;
        }
        //printing help message for operation that was passed
        //as an argument
        printf ("%s \n\n", operation->description);
    }
    return true;
}


/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Executes desired command

 @param[in]     argc number of command arguments
 @param[in]     argv command arguments
*/
/* ----------------------------------------------------------------------------
 */
void terminalExecuteCommand(uint8_t argc, char **argv)
{
    //searching command
    commandStruct *commandToExecute = terminalFindCommand ( argv[0]);
    static bool commandInProgress = false;
    //if command was not found
    if(commandToExecute == NULL)
    {
        printf("Command not found: %s \n", argv[0]);
        return;
    }
    if(commandToExecute->callback == NULL) 
    {
        printf("Callback function does not exist! \n");
        return;
    }
    static const commandStruct *commandInProgressPointer = NULL;

    if((commandToExecute != commandInProgressPointer) && commandInProgress)
    {
        printf("Other command currently ongoing, stop command and try again\n");
        return;
    }
    commandInProgress = false;
    bool prevCommandStatus = commandToExecute->callback(argc, argv);
}

/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Parses received string

 @param[in]     buf buffer containing the line to parse
*/
/* ----------------------------------------------------------------------------
 */
void terminalParse(char *buf)
{
    static char *argument[MAX_ARGUMENTS_NUMBER];
    uint8_t argNum = 0;
    // operation must be a first token
    char *arg = strtok(buf, " \t\n\r");
    // cycle until there is no available tokens
    while(arg != NULL)
    {
         // Copying arguments
        argument[argNum++] = arg;
        arg = strtok (NULL, " \t\n\r");
    }
    terminalExecuteCommand (argNum, argument);
}
/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Reads the line via UART bytewisely
*/
/* ----------------------------------------------------------------------------
 */
void terminalGetChar(void)
{
    static uint32_t charCount = 0;
    static uint8_t buffer[MAX_STRING_LENGTH] = {0};
    int32_t recChar;
    if(( recChar = getchar()) == EOF)
    {
        return;
    }
    if((char)recChar == '\r')
    {
        // Command received
        // changing CR or LF symbol on EOL symbol to ensure correct work of string
        // functions
        buffer[charCount] = '\0';
        // resetting counter
        charCount = 0;
        // resetting counter
        terminalParse((char*)buffer);
    }
    else if(isprint(recChar))
    {
        buffer[charCount++] = (char)recChar;
    }
    if(charCount >= MAX_STRING_LENGTH)
    {
        charCount = MAX_STRING_LENGTH - 2;
    }
}

bool terminalGetInt(char *str, uint32_t *var)
{
    errno = 0;
    *var = strtol(str, NULL, 10);
    return (errno != ERANGE);
}

