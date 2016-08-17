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
    
    //set RB7 high
    PORTBbits.RB7 = 1;
    BYTE dummy = 0;
    
    while(LCD_Idle_Check() == false) {};
    
    
    
    while(1) 
    {
        
    };
    
    return;
}
