/*
 * File:   main.c
 * Author: Jacob7395
 *
 * Created on August 10, 2016, 12:13 PM
 */

#include "System_Initialization.h" 

//interup maniger
void interrupt INTERRUPT_InterruptManager (void)
{
   // interrupt handler
    if(PIE1bits.TMR2IE == 1 && PIR1bits.TMR2IF == 1)
    {
        PORTBbits.RB7 = 1;
        PIR1bits.TMR2IF = 0;
    }
}

void main(void) {
    
    //system setup
    SYSTEM_Initialization();
    
    //temp VDD/GND power
    //set pin 7&5 to output
    TRISB &= 0x5F;
    //set 7 HIGH while 5 is low/GND
    PORTB |= 0x80;
    
    BYTE address = 0x40;
    BYTE data[5];
    
    data[0] = 0x50;
    data[1] = 0x55;
    data[2] = 0x55;
    data[3] = 0xff;
    data[4] = 0xff;
    
    int state = i2c_Send(address, &data[0], 5);
    
    //timer two setup
    //sync to fosc/4;rising edge input;TMR2_clk enable sycronised;01000 mode (one shot)
    T2HLT       = 0b10101000;
    //0 on enable;1:4 prescaler (010); no postscaler (00000)
    T2CON       = 0b00100000;
    //select fosv/4 clk
    T2CLKCON    = 0b00000001;
    //then set time to clock speed to 1MHz meaning 1 tick is 1 microsecond
    //for test load the counter for a 100 microsecond second tick   
    //lead period register
    T2PR        = 200;
    
    //set RB7 high
    PORTBbits.RB7 = 1;
    BYTE dummy = 0;
    
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    PIE1bits.TMR2IE = 0;
    PIR1bits.TMR2IF = 0;
    
    while(1) 
    {      
        //enable time
        T2CONbits.ON = 1;
        //set RB7 low to indercate timer start
        PORTBbits.RB7 = 0;
        //wait for timer to end
        while(PIR1bits.TMR2IF == 0) {};
        
        PORTBbits.RB7 = 1;
        
        PIR1bits.TMR2IF = 0;
    };
    
    return;
}
