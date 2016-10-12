/*
 * File:   Timer.c
 * Author: Jacob7395
 *
 * Created on 12 August 2016, 13:53
 */


#include "Timer.h"

//Timer1 functions
inline void TIMER1_Start() {T1CONbits.ON = 1;};
inline void TIMER1_Stop () {T1CONbits.ON = 0;};
//timer1 setup
void TIMER1_Init()
{
    //TIMER1 setup
    //clk = external crystal; 0 prescaler; sync control
    T1CON  = 0b10000000;
    //no using gate control
    T1GCON = 0b00000000;
    //select external occsolator
    T1CONbits.T1OSCEN = 1;
    //interupt setup
    PIE1bits.TMR1IE = 1; //enalbe interupt
    PIR1bits.TMR1IF = 0; //clear flag
}
//starts 2 second timer callbacks
void Start_Timer()
{
    //clear the registers
    TMR1H = 0x00;
    TMR1L = 0x00;
    //enable timer interrupt and clear flag
    PIE1bits.TMR1IE = 1; //enalbe interupt
    PIR1bits.TMR1IF = 0; //clear flag
    //start the timer
    TIMER1_Start();
}
//Timer3 functions
inline void TIMER3_Start() {T3CONbits.ON = 1;};
inline void TIMER3_Stop () {T3CONbits.ON = 0;};
//timer1 setup
void TIMER3_Init()
{
    //TIMER1 setup
    //clk = fosc/4; 1/4 prescaler;no secondery ocillator; sync control
    T3CON  = 0b00010000;
    //no using gate control
    T3GCON = 0b00000000;
    //interupt setup
    TIMER3_IE = 0; //enalbe interupt
    TIMER3_IF = 0; //clear flag
}

void TIMER3_BlockOut (HWORD Micro_Seconds)
{
    if(T3CONbits.ON) 
    {
        TIMER3_Stop();
    };
    
    TIMER3_IE = 0; //disable interrupt
    TIMER3_IF = 0; //clear flag
    
    //set lower and upper timer bytes
    int Time  =  0xffff - Micro_Seconds;
    BYTE LOW  =  Time       & 0x00ff;
    BYTE HIGH = (Time >> 8) & 0x00ff;
    
    //load the registers
    TMR3H = HIGH;
    TMR3L = LOW;
    //start the timer
    TIMER3_Start();
    //wait for timer to end
    while(TIMER3_IF == 0) {};
    //end timer and reset flags
    TIMER3_Stop();
    
    TIMER3_IF = 0; //clear flag
    TIMER3_IE = 1; //enable interrupt   
}
//timer 5
//Timer1 functions
inline void TIMER5_Start() {T5CONbits.ON = 1;};
inline void TIMER5_Stop () {T5CONbits.ON = 0;};
//timer1 setup
void TIMER5_Init()
{
    //TIMER5 setup
    //clk = fosc/4; 1/8 prescaler;no secondery ocillator; sync control
    T5CON  = 0b00110000;
    //no using gate control
    T5GCON = 0b00000000;
    //interupt setup
    TIMER5_IE = 0; //disable interupt
    TIMER5_IF = 0; //clear flag
}

void TIMER5_BlockOut (HWORD Micro_Seconds)
{
    if(T5CONbits.ON) 
    {
        TIMER5_Stop();
    };
    
    TIMER5_IE = 0; //disable interrupt
    TIMER5_IF = 0; //clear flag
    
    //set lower and upper timer bytes
    int Time  =  0xffff - Micro_Seconds;
    BYTE LOW  =  Time       & 0x00ff;
    BYTE HIGH = (Time >> 8) & 0x00ff;
    
    //load the registers
    TMR5H = HIGH;
    TMR5L = LOW;
    //start the timer
    TIMER5_Start();
    //wait for timer to end
    while(TIMER5_IF == 0) {};
    //end timer and reset flags
    TIMER5_Stop();
    
    TIMER5_IF = 0; //clear flag
    TIMER5_IE = 1; //enable interrupt   
}