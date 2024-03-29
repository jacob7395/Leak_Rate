/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCD_H
#define	LCD_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdbool.h>


//a BIT
typedef bool BIT;
//a BYTE
typedef unsigned char   BYTE;
//a half word (unsigned 16 bit)
typedef unsigned short HWORD;

#define LCD_ADDRESS   0x40
#define LCD_BACKLIGHT 0b00010000
#define LCD_ENABLE    0b00100000

#define LCD_ENABLE_LOW    0b11111011


#define LCD_PAGE_COUNT 5
//holds LCD page status
typedef enum
{
    P_HOME          = 0, //LCDS home page 
    P_TEST          = 1, //LCDS test setup page 
    P_TEST_CON      = 2, //Test conferm page
    P_TEST_TIMER    = 3, //Test timer page
    P_CANCLE_TEST   = 4, //page to cancle test
    P_IDLE          = 5, //Blank page
            
}LCD_PAGE_ID;

//functions
void LCD_Init();
//returns 1 when LCD is idle
bool LCD_Idle_Check();
//incroment or decorment the line 1 for incroment 0 for decroment
void LCD_Incroment_Decroment_Line(BIT Incroment_Decroment);
//used by dile to interact with LCD
void LCD_Manager(BYTE Command);
//decroment globle time
inline void LCD_Dectoment_Timer();
//loat end and start vac
void Load_Start_Vac(float vac);
void Load_End_Vac(float vac);

#endif	/* LCD_H */

