#ifndef TIMER_CTRL_H
#define TIMER_CTRL_H

typedef void (*fPointer)(void); 

typedef struct timerHandler
{
    uint8_t id;                  //timer id, used for deletion
    uint32_t periodMs;           //timer period is milliseconds
    uint32_t counter;            //counter for checking whether timer callback needs to be called
    fPointer callback;           //function to call after each period
    struct timerHandler *next;   //next timer handler        
}timerHandler;

void timerInit(void);
void timerDelayMs(uint32_t delayTime);
void timerPerformCheck(void);
void timerAdd(timerHandler *timerToAdd);
void timerRemove(timerHandler *timerToRemove);
#endif
