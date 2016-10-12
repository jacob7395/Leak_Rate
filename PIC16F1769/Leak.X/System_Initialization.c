/*
 * File:   System_Initialization.c
 * Author: Jacob7395
 *
 * Created on 12 August 2016, 13:21
 */

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF    // Watchdog Timer Enable->WDT disabled
#pragma config PWRTE = OFF    // Power-up Timer Enable->PWRT disabled
#pragma config MCLRE = OFF    // MCLR Pin Function Select->MCLR/VPP pin function is digital input
#pragma config CP = OFF    // Flash Program Memory Code Protection->Program memory code protection is disabled
#pragma config BOREN = OFF    // Brown-out Reset Enable->Brown-out Reset disabled
#pragma config CLKOUTEN = OFF    // Clock Out Enable->CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
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

#include "System_Initialization.h"

void CLOCK_Init();
void PORT_Init();

//runs all needed setups
void SYSTEM_Initialization()
{
    CLOCK_Init();
    PORT_Init();
    I2C_Init();
    TIMER3_Init();
    TIMER1_Init();  
    
    //wait for 1 seconds befor LCD Init to prevent line notice at bootip
    for(int i = 0; i < 125; i++)
    {
        TIMER5_BlockOut(10000);
    }
    
    LCD_Init();
    
    //enable global and periphral interrupts
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    
    //wait for LCD to setup
    while(LCD_Idle_Check() == false) {};
    //enable periphral interrupts
    INTERRUPT_PinInterruptEnable();
    
    DIAL_init();
    
    return;
}
//clock setup
void CLOCK_Init()
{
    // SCS FOSC; SPLLEN disabled; IRCF 16MHz_HF; 
    OSCCON = 0x78;
    // SOSCR disabled; 
    OSCSTAT = 0x00;
    // TUN 0; 
    OSCTUNE = 0x00;
}
// IO port setup
void PORT_Init()
{
    LATB = 0x0;
    LATA = 0x0;
    LATC = 0x0;
    WPUA = 0x30;
    WPUB = 0xF0;
    WPUC = 0xFF;
    ANSELA = 0x17;
    ANSELB = 0xA0;
    ANSELC = 0xCF;
    TRISB = 0xF0;
    TRISC = 0xFF;
    TRISA = 0x3F;
    
    return;
}