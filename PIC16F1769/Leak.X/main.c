/*
 * File:   main.c
 * Author: Jacob7395
 *
 * Created on August 10, 2016, 12:13 PM
 */

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF    // Watchdog Timer Enable->WDT disabled
#pragma config PWRTE = ON    // Power-up Timer Enable->PWRT disabled
#pragma config MCLRE = ON    // MCLR Pin Function Select->MCLR/VPP pin function is digital input
#pragma config CP = OFF    // Flash Program Memory Code Protection->Program memory code protection is disabled
#pragma config BOREN = OFF    // Brown-out Reset Enable->Brown-out Reset disabled
#pragma config CLKOUTEN = ON    // Clock Out Enable->CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
#pragma config IESO = OFF    // Internal/External Switchover Mode->Internal/External Switchover Mode is disabled
#pragma config FCMEN = OFF    // Fail-Safe Clock Monitor Enable->Fail-Safe Clock Monitor is disable

// CONFIG2
#pragma config WRT = OFF    // Flash Memory Self-Write Protection->Write protection off
#pragma config PPS1WAY = OFF    // Peripheral Pin Select one-way control->The PPSLOCK bit cannot be cleared once it is set by software
#pragma config ZCD = OFF    // Zero-cross detect disable->Zero-cross detect circuit is disabled at POR
#pragma config PLLEN = OFF    // Phase Lock Loop enable->4x PLL is always enabled
#pragma config STVREN = ON    // Stack Overflow/Underflow Reset Enable->Stack Overflow or Underflow will cause a Reset
#pragma config BORV = LO    // Brown-out Reset Voltage Selection->Brown-out Reset Voltage (Vbor), low trip point selected.
#pragma config LPBOR = OFF    // Low-Power Brown Out Reset->Low-Power BOR is disabled
#pragma config LVP = ON    // Low-Voltage Programming Enable->Low-voltage programming enabled

#include <xc.h>
#include <stdbool.h>

#include "i2c.h"

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
    //setup clock speed register
    OSCCON = 0x7F;
    
    LATB = 0x0;
    LATA = 0x0;
    LATC = 0x0;
    WPUA = 0x3F;
    WPUB = 0xF0;
    WPUC = 0xFF;
    ANSELA = 0x17;
    ANSELB = 0xA0;
    ANSELC = 0xCF;
    TRISB = 0xF0;
    TRISC = 0xFF;
    TRISA = 0x37;
    
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
    
    i2c_Init();
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
