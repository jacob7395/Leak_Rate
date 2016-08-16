/*
 * File:   LCD.c
 * Author: Jacob7395
 *
 * Created on 15 August 2016, 10:43
 */

#include "LCD.h"
#include "i2c.h"
#include "Timer.h"

#include <stdbool.h>

bool Line_Open = false;

void LCD_Begin_Coms()
{
    //open i2c line with a set address
    i2c_Open(LCD_ADDRESS);
    //set line status as open
    Line_Open = true;
}

void LCD_End_Coms()
{
    //close i2c line
    i2c_Close();
    //set line status as closed
    Line_Open = false;
}

void LCD_Send_Byte(BYTE data)
{
    if(Line_Open == false) {LCD_Begin_Coms();};
    
    BYTE data_array[3];
    
    data |= LCD_BACKLIGHT;
    
    data_array[0] = data & (!LCD_ENABLE); //sent the data with display enabled and enable low
    data_array[1] = data &   LCD_ENABLE;
    data_array[2] = data & (!LCD_ENABLE);
    
    i2c_Send_String(&data_array[0], 3);
}

void LCD_8bit_Send_Control(BYTE data)
{
    if(Line_Open == false) {LCD_Begin_Coms();};
    
    BYTE data_array[3];
    
    data = 0b00000001 | (data & 0xf0);
    
    data_array[0] = data; //sent the data with display enabled and enable low
    data_array[1] = data | LCD_ENABLE;// pulse enable high
    data_array[2] = data;
    
    i2c_Send_String(&data_array[0], 3);
}

void LCD_cmd(BYTE data)
{
    if(Line_Open == false) {LCD_Begin_Coms();};
    
    BYTE data_array[6];
    
    BYTE temp_data = 0x0c | (data & 0xf0);
    
    data_array[0] = temp_data; //sent the data with display enabled and enable high
    data_array[1] = temp_data & LCD_ENABLE_LOW;// pulse enable low
    data_array[2] = temp_data;
    
    temp_data = 0x0c | ((data << 4) & 0xf0);
    
    data_array[3] = temp_data; //sent the data with display enabled and enable high
    data_array[4] = temp_data & LCD_ENABLE_LOW;// pulse enable low
    data_array[5] = temp_data;
    
    i2c_Send_String(&data_array[0], 6);
    TIMER1_Wait(5000);
}

void LCD_data(BYTE data)
{
    if(Line_Open == false) {LCD_Begin_Coms();};
    
    BYTE data_array[6];
    
    BYTE temp_data = 0x0d | (data & 0xf0);
    
    data_array[0] = temp_data; //sent the data with display enabled and enable high
    data_array[1] = temp_data & LCD_ENABLE_LOW;// pulse enable low
    data_array[2] = temp_data;
    
    temp_data = 0x0d | ((data << 4) & 0xf0);
    
    data_array[3] = temp_data; //sent the data with display enabled and enable high
    data_array[4] = temp_data & LCD_ENABLE_LOW;// pulse enable low
    data_array[5] = temp_data;
    
    i2c_Send_String(&data_array[0], 6);
    TIMER1_Wait(5000);
}

void LCD_Reset()
{
    BYTE data = 0x00;
 
    if(Line_Open == false) {LCD_Begin_Coms();};
    
    data = 0x0c;
    i2c_Send_Byte(data);
    TIMER1_Wait(20000);
    
    data = 0x3c;
    i2c_Send_Byte(data);
    TIMER1_Wait(80);
    
    for(int i = 0; i < 3; i ++)
    {
        data = 0x38;
        i2c_Send_Byte(data);
        TIMER1_Wait(80);
    
        data = 0x3c;
        i2c_Send_Byte(data);
        TIMER1_Wait(5000);
    }
    
    data = 0x2c;
    i2c_Send_Byte(data);
    TIMER1_Wait(80);
    
    data = 0x28;
    i2c_Send_Byte(data);
    TIMER1_Wait(80);
    
    data = 0x2c;
    i2c_Send_Byte(data);
    TIMER1_Wait(5000);
}

void LCD_Init()
{ 
    LCD_Reset();
    
    LCD_cmd(0x28);
    LCD_cmd(0x08);
    LCD_cmd(0x0A);
    LCD_cmd(0x06);
    LCD_cmd(0x01);
    
    LCD_data(0x41);
    
    i2c_Close();
}
