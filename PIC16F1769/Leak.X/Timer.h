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
#ifndef TIMER_H
#define	TIMER_H

#include <xc.h> // include processor files - each processor file is guarded.  

//a BYTE
typedef unsigned char  BYTE;
//a half word (unsigned 16 bit)
typedef unsigned short HWORD;

//function decleration

//timer1 setup and control functions
//due to complecations with the external occolator timer1 functions use timer3
/* used to time LCD commands:
 * with interrupts enabled calling the LCD_ISR
 * 16 bit register
 * used 4MHz clock with 1:4 pre scaler
 * 1 tic = 1 micro second
 * Ranges form 0-65535 Micro Seconds
 */
#define TIMER1_IF PIR1bits.TMR1IF
#define TIMER1_IE PIE1bits.TMR1IE

void TIMER1_Init();
void TIMER1_Callback(HWORD Micro_Seconds);
inline void TIMER1_Stop ();
//timer3 setup and control functions
/* used as a generic block out timer not to be usied in itterupts:
 * 16 bit register
 * used 4MHz clock with 1:4 pre scaler
 * 1 tic = 1 micro second
 * Ranges form 0-65535 Micro Seconds
 */
#define TIMER3_IF PIR4bits.TMR3IF
#define TIMER3_IE PIE4bits.TMR3IE

void TIMER3_Init();
void TIMER3_BlockOut(HWORD Micro_Seconds);
//end of timer3
//timer5 setup and control functions
/* used as a generic wait timer:
 * 16 bit register
 * used 4MHz clock with 1:4 pre scaler
 * 1 tic = 2 micro second
 * Ranges form 2-65535 Micro Seconds
 */
#define TIMER5_IF PIR4bits.TMR5IF
#define TIMER5_IE PIE4bits.TMR5IE

void TIMER5_Init();
void TIMER5_BlockOut(HWORD Micro_Seconds);

#endif	/* XC_HEADER_TEMPLATE_H */

