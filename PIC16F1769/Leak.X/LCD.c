/*
 * File:   LCD.c
 * Author: Jacob7395
 *
 * Created on 15 August 2016, 10:43
 */

#include "LCD.h"
#include "i2c.h"
#include "Timer.h"

#include <string.h>
#include <stdbool.h>

//holds the LCD status for the IRS
typedef enum
{
    M_IDLE, //LCD not in use   
    M_NOT_SETUP, //set on boot-up  
    M_SET_PAGE, //used when LCD is setting page   
            
}LCD_MASTER_STATES;
//hold infomation about the LCD Page
typedef struct
{
    BYTE    PageID;
    
    BYTE    Number_Of_Lines;
    BYTE    Current_Line;
    
    BYTE    Line_Text[16][16];
} LCD_PAGE;
//hold infomation about the LCD
typedef struct
{
    bool    i2c_line_open;
    bool    LCD_setup;
    bool    LCD_IE;
    
    BIT     LCD_LINE;
    BYTE    LCD_Cursor_Position;
    
    BYTE    LCD_Line_Value[16][2];
    
} LCD_OBJECT;

static LCD_MASTER_STATES                   LCD_state         = M_NOT_SETUP;
static LCD_PAGE_STATES                     LCD_Page          = P_HOME;
static LCD_PAGE_STATES                     LCD_Desired_Page  = P_HOME;
static LCD_OBJECT                          LCD_Object;

//define LCD Page
static LCD_PAGE                            P_Home_Object;

bool Line_Open = false;


//open I2C line to LCD address
void LCD_Open_Coms()
{   
    //open i2c line with a set address
    i2c_Open(LCD_ADDRESS);
    //set line status as open
    LCD_Object.i2c_line_open = true;
}
//close I2C line to LCD address
void LCD_Close_Coms()
{
    //close i2c line
    i2c_Close();
    //set line status as closed
    LCD_Object.i2c_line_open = false;
}

void LCD_Send_Byte(BYTE data)
{
    if(Line_Open == false) {LCD_Open_Coms();};
    
    BYTE data_array[3];
    
    data |= LCD_BACKLIGHT;
    
    data_array[0] = data & (!LCD_ENABLE); //sent the data with display enabled and enable low
    data_array[1] = data &   LCD_ENABLE;
    data_array[2] = data & (!LCD_ENABLE);
    
    i2c_Send_String(data_array, 3);
}

void LCD_8bit_Send_Control(BYTE data)
{
    if(Line_Open == false) {LCD_Open_Coms();};
    
    BYTE data_array[3];
    
    data = 0b00000001 | (data & 0xf0);
    
    data_array[0] = data; //sent the data with display enabled and enable low
    data_array[1] = data | LCD_ENABLE;// pulse enable high
    data_array[2] = data;
    
    i2c_Send_String(&data_array[0], 3);
}
//sends data to the command register in 4 bit mode
void LCD_cmd(BYTE data)
{   
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
    TIMER1_BlockOut(5000);
}

void LCD_data(BYTE data)
{
    if(Line_Open == false) {LCD_Open_Coms();};
    
    BYTE data_array[6];
    
    BYTE temp_data = 0x0d | (data & 0xf0);
    
    data_array[0] = temp_data; //sent the data with display enabled and enable high
    data_array[1] = temp_data & LCD_ENABLE_LOW;// pulse enable low
    data_array[2] = temp_data;
    
    temp_data = 0x0d | ((data << 4) & 0xf0);
    
    data_array[3] = temp_data; //sent the data with display enabled and enable high
    data_array[4] = temp_data & LCD_ENABLE_LOW;// pulse enable low
    data_array[5] = temp_data;
    
    i2c_Send_String(data_array, 6);
    TIMER1_Callback(5000);
}
//set the LCD in 4 bit mode
void LCD_Reset()
{
    BYTE data = 0x00;
 
    if(Line_Open == false) {LCD_Open_Coms();};
    
    i2c_Send_Byte(0x0c);
    TIMER1_Callback(20000);
    
    i2c_Send_Byte(0x3c);
    TIMER1_Callback(80);
    
    for(int i = 0; i < 3; i ++)
    {
        i2c_Send_Byte(0x38);
        TIMER1_Callback(80);
    
        i2c_Send_Byte(0x3c);
        TIMER1_Callback(5000);
    }
    
    i2c_Send_Byte(0x2c);
    TIMER1_Callback(80);
    
    i2c_Send_Byte(0x28);
    TIMER1_Callback(80);
    
    i2c_Send_Byte(0x2c);
    TIMER1_Callback(5000);
}

inline void LCD_Clear()
{
    LCD_cmd(0x01); 
    TIMER1_Callback(1500);
}

inline void LCD_Home()
{
    LCD_cmd(0x02); 
    TIMER1_Callback(1500);
}
//initilize each LCD page including LINE data and ID tag
void LCD_Init_Pages()
{ 
    //home page init
    P_Home_Object.PageID = P_HOME;
    
    P_Home_Object.Current_Line = 0;
    P_Home_Object.Number_Of_Lines = 3;
    
    P_Home_Object.Line_Text[0][0] = "test1";
    P_Home_Object.Line_Text[0][1] = "test2";
    P_Home_Object.Line_Text[0][2] = "test3";
}

void LCD_Init()
{ 
    //setup LCD values
    LCD_Object.LCD_setup            = false;
    LCD_Object.i2c_line_open        = false;
    LCD_Object.LCD_IE               = true;
    LCD_Object.LCD_LINE             = 0;
    LCD_Object.LCD_Cursor_Position  = 0;
    //set LCD line values to ' ' ie null
    memset(LCD_Object.LCD_Line_Value, ' ', sizeof(LCD_Object.LCD_Line_Value[0][0]) * 16 * 2);
    //setup each lcd page 
    LCD_Init_Pages();
    //set the LCD in 4 bit mode
    LCD_Reset();
            
    LCD_cmd(0x06); //I/D high; SH low
    LCD_cmd(0x0E); //Display on; Cursor on; Cursor Blink Off
    LCD_cmd(0x28); //4 bit bus;2 line display;5x8 dot matrix
    
    LCD_Clear(); //clear LCD screen
            
    LCD_state = M_IDLE; //set master status to idle
}

void LCD_Set_Page(LCD_PAGE_STATES Page)
{
    LCD_Desired_Page = Page;
    LCD_state    = M_SET_PAGE;
    TIMER1_Callback(10);
}

void LCD_ICR()
{
    //check i2c has opened the line with the LCD
    if(LCD_Object.i2c_line_open == false) {LCD_Open_Coms();};
    //witch between LCD states
    switch(LCD_state)
    {
        case M_SET_PAGE:
            //check current page = set page
            if(LCD_Desired_Page == LCD_Page)
            {
                LCD_state = M_IDLE;
                break;
            }
            
            switch(LCD_state)
            {
                case P_HOME:
                    
                    
                    
                    break;
            }
            
            break;
    }
}
//return true if LCD is idle
//return false is not
bool LCD_Idle_Check()
{
    if(LCD_state == M_IDLE) 
        return true;
    else 
        return false;
}
