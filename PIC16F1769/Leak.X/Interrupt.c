/*
 * File:   Interrupt.c
 * Author: Jacob7395
 *
 * Created on 12 August 2016, 15:24
 */


#include "Interrupt.h"
//files using interrupts
#include "Timer.h"
#include "LCD.h"
#include "Rotary_Dial.h"

//interrupt manager
void interrupt INTERRUPT_InterruptManager(void)
{
   // interrupt handler for timer 1
    if (TIMER1_IE == 1 && TIMER1_IF == 1)
    {
        //reset interrupt flag
        TIMER1_IF = 0;
        //decrse timer by 2 seconds
        LCD_Dectoment_Timer();
        //decroment time
        LCD_Manager(3);
    }
    // interrupt for pin C5
    if (IOCCNbits.IOCCN5  == 1 && IOCCFbits.IOCCF5 == 1)
    {
        DIAL_ICR();
        
        IOCCFbits.IOCCF5 = 0;
    }
    // interrupt for pin C7
    if (IOCCPbits.IOCCP7 == 1 && IOCCFbits.IOCCF7 == 1)
    {
        DIAL_ICR();
        
        IOCCFbits.IOCCF7 = 0;
    }
}
