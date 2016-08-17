/*
 * File:   Timer.c
 * Author: Jacob7395
 *
 * Created on 12 August 2016, 13:53
 */


#include "Timer.h"

//Timer1 functions
inline void TIMER1_Start() {T1CON = 0b00100001;};
inline void TIMER1_Reset() {};
inline void TIMER1_Stop () {T1CONbits.ON = 0;};
//timer1 setup
void TIMER1_Init()
{
    //TIMER1 setup
    //clk = fosc/4; 1/4 prescaler;no secondery ocillator; sync control
    T1CON  = 0b00100000;
    //no using gate control
    T1GCON = 0b00000000;
    //interupt setup
    PIE1bits.TMR1IE = 1; //enalbe interupt
    PIR1bits.TMR1IF = 0; //clear flag
}

void TIMER1_Callback (HWORD Micro_Seconds)
{
    //set lower and upper timer bytes
    int Time  =  0xffff - Micro_Seconds;
    BYTE LOW  =  Time       & 0x00ff;
    BYTE HIGH = (Time >> 8) & 0x00ff;
    
    //load the registers
    TMR1H = HIGH;
    TMR1L =  LOW;
    //start the timer
    TIMER1_Start();
}

void TIMER1_BlockOut (HWORD Micro_Seconds)
{
    PIE1bits.TMR1IE = 0; //disable interrupt
    PIR1bits.TMR1IF = 0; //clear flag
    
    //set lower and upper timer bytes
    int Time  =  0xffff - Micro_Seconds;
    BYTE LOW  =  Time       & 0x00ff;
    BYTE HIGH = (Time >> 8) & 0x00ff;
    
    //load the registers
    TMR1H = HIGH;
    TMR1L =  LOW;
    //start the timer
    TIMER1_Start();
    //wait for timer to end
    while(PIR1bits.TMR1IF == 0) {};
    
    PIR1bits.TMR1IF = 0; //clear flag
    PIE1bits.TMR1IE = 0; //enable interrupt   
}

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
    PIE1bits.TMR2IE = 0; //disable interupt
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
