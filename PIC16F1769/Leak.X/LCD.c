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
    M_IDLE,             //LCD not in use   
    M_NOT_SETUP,        //set on boot-up  
    M_SET_PAGE,         //used when LCD is setting page 
    M_SCREEN_UPDATE,    //signal screen requires update
    M_SCREEN_WRITE,     //flag to say LCD is writing
            
}LCD_MASTER_STATES;
//hold infomation about the LCD Page
typedef struct
{   
    BYTE        Number_Of_Lines;
    BYTE        Current_Line;
    
    BYTE        Line_Text[6][16];
} LCD_PAGE;
//hold infomation about the LCD screen
typedef struct
{
    HWORD       Cursor_Y;
    HWORD       Cursor_X;
} LCD_SCREEN;
//hold infomation about the LCD
typedef struct
{
    bool        i2c_line_open;
    bool        Setup;
    bool        IE;
    
    LCD_SCREEN  Screen;
    
    BYTE        Number_Of_Pages;
    LCD_PAGE    Page[LCD_PAGE_COUNT];
    LCD_PAGE_ID Current_Page;
    LCD_PAGE_ID Desired_Page;
} LCD_OBJECT;

static LCD_MASTER_STATES                   LCD_state         = M_NOT_SETUP;
static LCD_OBJECT                          LCD_Object;

//internal function declaration
void LCD_Set_Page(LCD_PAGE_ID Page);

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
    if(LCD_Object.i2c_line_open == false) {LCD_Open_Coms();};
    
    BYTE data_array[3];
    
    data |= LCD_BACKLIGHT;
    
    data_array[0] = data & (!LCD_ENABLE); //sent the data with display enabled and enable low
    data_array[1] = data &   LCD_ENABLE;
    data_array[2] = data & (!LCD_ENABLE);
    
    i2c_Send_String(data_array, 3);
}

void LCD_8bit_Send_Control(BYTE data)
{
    if(LCD_Object.i2c_line_open == false) {LCD_Open_Coms();};
    
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
    if(LCD_Object.i2c_line_open == false) {LCD_Open_Coms();}; //check line state
    
    BYTE data_array[6];
    
    BYTE temp_data = 0x0c | (data & 0xf0);
    
    data_array[0] = temp_data; //sent the data with display enabled and enable high
    data_array[1] = temp_data & LCD_ENABLE_LOW;// pulse enable low
    
    temp_data = 0x0c | ((data << 4) & 0xf0);
    
    data_array[2] = temp_data; //sent the data with display enabled and enable high
    data_array[3] = temp_data & LCD_ENABLE_LOW;// pulse enable low
    data_array[4] = temp_data;
    
    i2c_Send_String(&data_array[0], 5);
}

void LCD_data(BYTE data)
{
    //check line status
    if(LCD_Object.i2c_line_open == false) {LCD_Open_Coms();};
    //check for illegal character
    if(data == 0x00) {data = ' ';};
    
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
    TIMER1_Callback(1000);
}
//set the LCD in 4 bit mode
void LCD_Reset()
{
    BYTE data = 0x00;
 
    if(LCD_Object.i2c_line_open == false) {LCD_Open_Coms();};
    
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
    LCD_Object.Page[P_HOME].Current_Line    = 0;
    LCD_Object.Page[P_HOME].Number_Of_Lines = 3;
    
    memset(LCD_Object.Page[P_HOME].Line_Text, ' ', sizeof(LCD_Object.Page[0].Line_Text[0][0]) * LCD_Object.Page[0].Number_Of_Lines * 16);
    
    strcpy(LCD_Object.Page[P_HOME].Line_Text[0], "test1");
    strcpy(LCD_Object.Page[P_HOME].Line_Text[1], "test2");
    strcpy(LCD_Object.Page[P_HOME].Line_Text[2], "test3");
}

void LCD_Init()
{ 
    //setup LCD values
    LCD_Object.Setup                = false;
    LCD_Object.i2c_line_open        = false;
    LCD_Object.IE                   = true;
    LCD_Object.Screen.Cursor_Y      = 0;
    LCD_Object.Screen.Cursor_X      = 0;
    LCD_Object.Current_Page         = P_IDLE;
    LCD_Object.Desired_Page         = P_IDLE;
    //may or may not work needs testing
    LCD_Object.Number_Of_Pages  = sizeof(LCD_Object.Page)/sizeof(LCD_Object.Page[0]);
    //alternative
    LCD_Object.Number_Of_Pages  = LCD_PAGE_COUNT;
    //setup each LCD page 
    LCD_Init_Pages();
    
    //set the LCD in 4 bit mode
    LCD_Reset();
            
    LCD_cmd(0x06); //I/D high; SH low
    TIMER1_BlockOut(5000); //wait for cmd to be set (this is not in the function so LCD_cmd can be used with the ISR)
    LCD_cmd(0x0E); //Display on; Cursor on; Cursor Blink Off
    TIMER1_BlockOut(5000); //wait for cmd to be set
    LCD_cmd(0x28); //4 bit bus;2 line display;5x8 dot matrix
    TIMER1_BlockOut(5000); //wait for cmd to be set
    
    LCD_Clear(); //clear LCD screen
            
    LCD_state = M_IDLE; //set master status to idle
    
    LCD_Set_Page(P_HOME); //set the home page
}
//set page do requested page
void LCD_Set_Page(LCD_PAGE_ID Page)
{
    LCD_Object.Desired_Page = Page;
    LCD_state               = M_SET_PAGE;
    TIMER1_Callback(10);
}

void LCD_Set_Cursor_Line(BYTE line)
{
    switch (line)
    {
        case 1:
            LCD_cmd(0x80);  
            LCD_Object.Screen.Cursor_X = 0;
            LCD_Object.Screen.Cursor_Y = 0;
            TIMER1_Callback(50);
            break;
        case 2:
            LCD_cmd(0xC0);
            LCD_Object.Screen.Cursor_X = 0;
            LCD_Object.Screen.Cursor_Y = 1;
            TIMER1_Callback(50);
            break;
    }
}
//incroment or decroment line count 0 for decroment 1 for incroment
void LCD_Incroment_Decroment_Line(BIT Incroment_Decroment)
{
    switch (Incroment_Decroment)
    {
        case 0:
            if(LCD_Object.Page[LCD_Object.Current_Page].Current_Line - 1 != -1)
            {
                LCD_Object.Page[LCD_Object.Current_Page].Current_Line--;
                LCD_state = M_SCREEN_UPDATE;
                TIMER1_Callback(10);
            }
            break;
        case 1:
            if(LCD_Object.Page[LCD_Object.Current_Page].Current_Line + 2 != LCD_Object.Page[LCD_Object.Current_Page].Number_Of_Lines)
            {
                LCD_Object.Page[LCD_Object.Current_Page].Current_Line++;
                LCD_state = M_SCREEN_UPDATE;
                TIMER1_Callback(10);
            }
            break;
    }
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
            if(LCD_Object.Desired_Page == LCD_Object.Current_Page)
            {
                LCD_state = M_IDLE;
                break;
            }
            //set page wanted
            LCD_Object.Current_Page = LCD_Object.Desired_Page;
            //reset cursor position and line values
            LCD_Object.Screen.Cursor_X = 0;
            LCD_Object.Screen.Cursor_Y = 0;
            LCD_Object.Page[LCD_Object.Desired_Page].Current_Line = 0;
            //clear screen this has a callback function
            LCD_Clear();
            //set states to write mode
            LCD_state = M_SCREEN_WRITE;
            
            break;
            //reset values ready for screen write
        case M_SCREEN_UPDATE:
            //reset cursor position
            LCD_Object.Screen.Cursor_X = 0;
            LCD_Object.Screen.Cursor_Y = 0;
            //clear screen this has a callback function
            LCD_Clear();
            //set states to write mode
            LCD_state = M_SCREEN_WRITE;
            break;
        case M_SCREEN_WRITE:
            //write the value for the current cursor position and current line
            LCD_data(LCD_Object.Page[LCD_Object.Current_Page].Line_Text[LCD_Object.Screen.Cursor_Y + LCD_Object.Page[LCD_Object.Current_Page].Current_Line][LCD_Object.Screen.Cursor_X]);
            //increment cursor if still on screen or change line else end callback
            if      (LCD_Object.Screen.Cursor_X < 15)
                LCD_Object.Screen.Cursor_X++;
            else if (LCD_Object.Screen.Cursor_Y < 1)
                LCD_Set_Cursor_Line(2);
            else
                LCD_state = M_IDLE;
            
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
