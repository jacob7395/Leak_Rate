/*
 * File:   Timer.c
 * Author: Jacob7395
 *
 * Created on 12 August 2016, 13:53
 */


#include "Timer.h"

//Timer2 functions
inline void TIMER2_Start() {T2CONbits.ON = 1;};
inline void TIMER2_Reset() {};
inline void TIMER2_Stop () {T2CONbits.ON = 0;};

//timer2 setup
void TIMER2_Init()
{
    //timer two setup
    //sync to fosc/4;rising edge input;TMR2_clk enable sycronised;01000 mode (one shot)
    T2HLT       = 0b10101000;
    //0 on enable;1:4 prescaler (010); no postscaler (00000)
    T2CON       = 0b00100000;
    //select fosv/4 clk
    T2CLKCON    = 0b00000001;
    //interupt setup
    PIE1bits.TMR2IE = 0; //no interupt
    PIR1bits.TMR2IF = 0; //clear flag
}

void TIMER2_Wait (BYTE Micro_Seconds)
{
    //load the register
    T2PR = Micro_Seconds;
    //start the timer
    TIMER2_Start();
    //wait for timer to end
    while(PIR1bits.TMR2IF == 0) {};
    //clear flag
    PIR1bits.TMR2IF = 0;
}
