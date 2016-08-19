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

//interrupt manager
void interrupt INTERRUPT_InterruptManager(void)
{
   // interrupt handler for timer 1
    if(PIE1bits.TMR1IE == 1 && PIR1bits.TMR1IF == 1)
    {
        //stop timer1
        TIMER1_Stop();
        //reset interrupt flag
        PIR1bits.TMR1IF = 0;
        //call the LCD ICR
        LCD_ICR();       
    }
}
