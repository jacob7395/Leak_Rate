/*
 * File:   Rotary_Dial.c
 * Author: Jacob7395
 *
 * Created on 22 August 2016, 10:41
 */

#include "Rotary_Dial.h"
#include "LCD.h"
#include "Timer.h"

#define SWT_IF IOCCFbits.IOCCF7
#define CLK_IF IOCCFbits.IOCCF5

//High low dial status
typedef enum
{
    D_HIGH = 1,
    D_LOW  = 0
}DIAL_LINE_STATE;
//Dial objectt
typedef struct
{
    BYTE Data_OLD;
    
    bool Invalid_State; //set high when clk is low and data is high(invalid state)
} DIAL_OBJECT;

static DIAL_LINE_STATE Dial_CLK = D_LOW;
static DIAL_LINE_STATE Dial_DTA = D_LOW;

static DIAL_OBJECT     Dial_Object;

//DIAL init
void DIAL_init()
{
    //set pins to input
    TRISC  |= 0b11100000;
    //disable analog reg
    ANSELC &= 0b00011111;
    //disable week pull up
    WPUC   &= 0b00011111;
    
    //enable interup pins on rising edge for C5(clk line) and C7(button line)
    //IOCCPbits.IOCCP5 = 1;
    IOCCPbits.IOCCP7 = 1;
    //enable change on falling edge for C5
    IOCCNbits.IOCCN5 = 1;
    //clear flags
    IOCCFbits.IOCCF5 = 0;
    IOCCFbits.IOCCF7 = 0;
    //set line state
    Dial_Object.Data_OLD = PORTCbits.RC6;
}
//DIAL interrupt servecis routine
void DIAL_ICR()
{    
    if(SWT_IF)
    {
        BIT SWT_Data = PORTCbits.RC7;
        
        LCD_Manager(0x00);
    }
    
    if(CLK_IF)
    {
        //define data
        BIT DTA_Data = 0;
        //set wait for a short period of time
        TIMER5_BlockOut(2000);
        //used to incroment decroment
        counter = 0xB2;
        
        if(PORTCbits.RC5 == 1)
        {
            return;
        }
        
        for(int i = 0; i < 10; i ++)
        {
            //read data
            if((DTA_Data = PORTCbits.RC6) == 1)
            {
                counter++;
            }
            else
            {
                counter--;
            }
            //incroment and decroment counter
            TIMER5_BlockOut(10);
        }
        //wait for LCD to be free
        while(LCD_Idle_Check() == false) {};
        //read the data line again then compare the data
        if     (counter < 0xB2)
        {     
            LCD_Manager(0x01);         
        }
        else if(counter > 0xB2)
        {
            LCD_Manager(0x02);
        }
    }
}
