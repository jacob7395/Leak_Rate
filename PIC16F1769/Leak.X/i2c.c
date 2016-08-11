/*
 * File:   i2c.c
 * Author: Jacob7395
 *
 * Created on 11 August 2016, 15:13
 */


#include "i2c.h"

//initilise registers for i2c comes
//RB4 used for SDA
//RB6 used for SCL
void i2c_Init()
{
    //set B4&B6 to input as needed for MSSP
    TRISB |= 0x50;
    
    RB4PPSbits.RB4PPS = 0x13;   //RB4->MSSP:SDA;
    RB6PPSbits.RB6PPS = 0x12;   //RB6->MSSP:SCL;
    SSPDATPPSbits.SSPDATPPS = 0x0C;   //RB4->MSSP:SDA;
    SSPCLKPPSbits.SSPCLKPPS = 0x0E;   //RB6->MSSP:SCL;

    // R_nW write_noTX; P stopbit_notdetected; S startbit_notdetected; BF RCinprocess_TXcomplete; SMP Standard Speed; UA dontupdate; CKE disabled; D_nA lastbyte_address; 
    SSP1STAT = 0x80;
    // SSPEN enabled; WCOL no_collision; CKP Idle:Low, Active:High; SSPM FOSC/4_SSPxADD_I2C; SSPOV no_overflow; 
    SSP1CON1 = 0x28;
    // ACKTIM ackseq; SBCDE disabled; BOEN disabled; SCIE disabled; PCIE disabled; DHEN disabled; SDAHT 100ns; AHEN disabled; 
    SSP1CON3 = 0x00;
    //for a 16Mhz clock to drive a 100Khz BRG the value is 0x27
    //this is shown in table 32-4 of the chip data sheet
    SSP1ADD  = 0x27;
}
//waits for SSP1IF to be set, manual poll insted of interup
//if SSP1IF is set return 0
//if SSP1IF is not set and time-out occures return 1 (not done time-out yet)
int i2c_wait()
{
    //wait till SSP1IF(may be an intterupt pin) is set high (bit shown in TABLE 32-3)
    //indecating the start condition was transmitted
    while(SSP1IF != 1) {};
    //clear SSP1IF pin
    SSP1IF = 0;
    
    return 0;
}
//
int i2c_Send_Byte(BYTE data)
{
    SSP1BUF = data;
    if(i2c_wait() != 0) {return 1;};
    return 0;
}
//protocol to sned data to an address using an array of data bytes
int i2c_Send(BYTE address, BYTE *data[], int number_of_bytes)
{
    //set start bit condition flag using SEN bit
    SSP1CON2bits.SEN = 1;
    //waits for start egnoligment if no start return error 1
    if(i2c_wait() != 0) {return 1;};
    //send address checking for error
    if(i2c_Send_Byte(address) != 0) {return 2;};
    //loop to send number of Bytes requested
    for(int i = 0; i < number_of_bytes; i++)
    {
        //send each Byte of data
        if(i2c_Send_Byte(data[i]) != 0) {return 3;};
    }
    //once all data send end i2c
    //set i2c stop by setting PEN(bit2) in SSP1CON2
    SSP1CON2bits.PEN = 0x01;
    //waits for start egnoligment if no start return error 1
    if(i2c_wait() != 0) {return 4;};
    //if all is succssessful return 0
    return 0;
}
