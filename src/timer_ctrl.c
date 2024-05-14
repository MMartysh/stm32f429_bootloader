#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "errorHandlers.h"
#include "timer_ctrl.h"
#include "terminal.h"

#define SYSTEM_CLK_DIVIDER 1600
static timerHandler *timerHead;
static uint32_t delayCounter = 0;
void timerInit(void)
{
    SysTick_Config(SystemCoreClock / SYSTEM_CLK_DIVIDER);
}
/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Do the counter check and call the timer function
*/
/* ----------------------------------------------------------------------------
 */
void timerPerformCheck(void)
{
    if(timerHead == NULL)
    {
        return;
    }
    //begin from the list head
    static timerHandler *head;
    //if head exists
    if (head != NULL)
    { 
        //if counter is 0
        if(head->counter == 0)
        {
            //call the function
            head->callback();
            //reset counter
            head->counter = head->periodMs;
        }
        //goto next node
        if(head->next != NULL)
        {
            head = head->next;
        }
    }
    else
    {
        //go to list head
        head = timerHead;
    }
}
/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Do the counter check and call the timer function

 @param[in]     timerToAdd timer handler to apeend to the list
*/
/* ----------------------------------------------------------------------------
 */
void timerAdd(timerHandler *timerToAdd)
{
    //begin from the list head
    timerHandler *head = timerHead;
    static uint32_t timerId = 0;
    timerToAdd->next = NULL;
    //if head exists
    if (head != NULL)
    { 
        //goto the last node
        while(head->next != NULL)
        {
            head = head->next;
        }
        //insert command at the end of the list
        head->next = timerToAdd;
        //set counter
        head->counter = head->periodMs;
        
    }
    else
    {
        //make command a head of the list
        timerHead = timerToAdd; 
    }
    head->id = ++timerId;
}
/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Remove timer from list

 @param[in]     timerToRemove timer handler to remove from list.
*/
/* ----------------------------------------------------------------------------
 */
void timerRemove(timerHandler *timerToRemove)
{
    //begin from the list head
    timerHandler *head = timerHead;
    timerHandler *prev = NULL;
    //goto the last node
    while(head != NULL)
    {
        if(head->id == timerToRemove->id)
        {
            prev->next = head->next;
            return;
        }
        else
        {
            prev = head;
            head = head->next;
        }
    }
}
/* ----------------------------------------------------------------------------
 */
/*!
 @brief         Interrupt handler
*/
/* ----------------------------------------------------------------------------
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
    if(delayCounter != 0){delayCounter--;}
    for(timerHandler *it = timerHead; it != NULL; it = it->next)
    {
        it->counter--;
    }
}

void timerDelayMs(uint32_t delayTime)
{
    delayCounter = delayTime;
    while(delayCounter != 0);
}