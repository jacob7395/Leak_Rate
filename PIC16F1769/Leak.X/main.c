/*
 * File:   main.c
 * Author: Jacob7395
 *
 * Created on August 10, 2016, 12:13 PM
 */

#include "System_Initialization.h" 

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
    
    //set RB7 high
    PORTBbits.RB7 = 1;
    BYTE dummy = 0;
    
    while(1) 
    {      
        PORTBbits.RB7 = 0;
        TIMER1_Wait(10000);
        PORTBbits.RB7 = 1;
        TIMER1_Wait(10000);
    };
    
    return;
}
