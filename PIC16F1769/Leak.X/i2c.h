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
#ifndef I2C_H
#define	I2C_H

#include <stdbool.h>
#include <xc.h>

//The following control bits are used in the I2C state machine to manage
//the I2C module and determine next states.
#define I2C_WRITE_COLLISION_STATUS_BIT         SSP1CON1bits.WCOL     // Defines the write collision status bit.
#define I2C_MODE_SELECT_BITS                   SSP1CON1bits.SSPM     // I2C Master Mode control bit.
#define I2C_MASTER_ENABLE_CONTROL_BITS         SSP1CON1bits.SSPEN    // I2C port enable control bit.

#define I2C_START_CONDITION_ENABLE_BIT         SSP1CON2bits.SEN      // I2C START control bit.
#define I2C_REPEAT_START_CONDITION_ENABLE_BIT  SSP1CON2bits.RSEN     // I2C Repeated START control bit.
#define I2C_RECEIVE_ENABLE_BIT                 SSP1CON2bits.RCEN     // I2C Receive enable control bit.
#define I2C_STOP_CONDITION_ENABLE_BIT          SSP1CON2bits.PEN      // I2C STOP control bit.
#define I2C_ACKNOWLEDGE_ENABLE_BIT             SSP1CON2bits.ACKEN    // I2C ACK start control bit.
#define I2C_ACKNOWLEDGE_DATA_BIT               SSP1CON2bits.ACKDT    // I2C ACK data control bit.
#define I2C_ACKNOWLEDGE_STATUS_BIT             SSP1CON2bits.ACKSTAT  // I2C ACK status bit.

//a BYTE
typedef unsigned char BYTE;

//function decloreations
//i2c port config
void I2C_Init();
//sends # bytes to address form data
int i2c_Send(BYTE address, BYTE *data[], int number_of_bytes);
//open i2c line with a set address
int i2c_Open(BYTE address);
//send a byte to the current open line
int i2c_Send_Byte(BYTE data);
//send a string down the i2c line
int i2c_Send_String(BYTE *data[], int number_of_bytes);
//close the line
int i2c_Close();

#endif	/* XC_HEADER_TEMPLATE_H */

