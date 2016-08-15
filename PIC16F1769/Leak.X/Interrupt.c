/*
 * File:   Interrupt.c
 * Author: Jacob7395
 *
 * Created on 12 August 2016, 15:24
 */


#include "Interrupt.h"

//interup maniger
void interrupt INTERRUPT_InterruptManager(void)
{
   // interrupt handler
    if(PIE1bits.TMR2IE == 1 && PIR1bits.TMR2IF == 1)
    {
        PIR1bits.TMR2IF = 0;
    }
}
