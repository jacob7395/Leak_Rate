/*
 * File:   LCD.c
 * Author: Jacob7395
 *
 * Created on 15 August 2016, 10:43
 */

#include "LCD.h"
#include "i2c.h"
#include "Timer.h"
#include "DAC_ADC.h"

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
    
    BYTE        Line_Text[2][16];
    
    bool        Cursor_On;
    bool        Yes_No_Check;
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
    
    int         Test_length;
    int         Stored_Test_Length;
    char        Time_select;
    bool        Confirm;
    
    float       Start_vac;
    float       End_vac;
} LCD_OBJECT;

static LCD_MASTER_STATES                   LCD_state         = M_NOT_SETUP;
static LCD_OBJECT                          LCD_Object;

//internal function declaration
void LCD_Set_Page(LCD_PAGE_ID Page);
void LCD_Screen_Write();
void LCD_data(BYTE data);
void LCD_cmd(BYTE data);
void LCD_Set_Cursor_Line(BYTE line);
void LCD_Write_Arrow(BYTE x, BYTE line);


//funtuon to load star and end vac cal
void Load_Start_Vac(float vac)
{
    LCD_Object.Start_vac = vac;
}

void Load_End_Vac(float vac)
{
    LCD_Object.End_vac = vac;
}
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
//function to write current test time
void LCD_Write_Test_Time()
{
    LCD_Set_Cursor_Line(2);
    
    int Temp_time = LCD_Object.Test_length;
    
    char time[3];
    
    time[0] = (Temp_time / 3600);
    Temp_time = Temp_time - (time[0] * 3600);
    time[1] = (Temp_time / 60);
    time[2] = Temp_time - (time[1] * 60);
    
    char num_length = 2;  
    char num[2];

    for(char i = 0; i <= 2; i++)
    {
        num[1] = time[i]/10;
        num[0] = time[i] - (num[1] * 10);
    
        LCD_data(num[1] + 48);
        LCD_data(num[0] + 48);
        if(i != 2)
            LCD_data(':');
    }
    //set curcer to correct time select
    LCD_cmd(0xC0 + 1 + (LCD_Object.Time_select * 3));
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
}
//set the LCD in 4 bit mode
void LCD_Reset()
{
    BYTE data = 0x00;
 
    if(LCD_Object.i2c_line_open == false) {LCD_Open_Coms();};
    
    i2c_Send_Byte(0x0c);
    TIMER5_BlockOut(20000);
    
    i2c_Send_Byte(0x3c);
    TIMER5_BlockOut(80);
    
    for(int i = 0; i < 3; i ++)
    {
        i2c_Send_Byte(0x38);
        TIMER5_BlockOut(80);
    
        i2c_Send_Byte(0x3c);
        TIMER5_BlockOut(5000);
    }
    
    i2c_Send_Byte(0x2c);
    TIMER5_BlockOut(80);
    
    i2c_Send_Byte(0x28);
    TIMER5_BlockOut(80);
    
    i2c_Send_Byte(0x2c);
    TIMER5_BlockOut(5000);
}

inline void LCD_Clear()
{
    LCD_cmd(0x01); 
    TIMER5_BlockOut(15000);
}

inline void LCD_Home()
{
    LCD_cmd(0x02); 
    TIMER5_BlockOut(15000);
}

inline void LCD_Set_Cursor(bool ON)
{
    if(ON)
        LCD_cmd(0b00001110);
    else
        LCD_cmd(0b00001100);
    
    TIMER5_BlockOut(15000);
}

//initilize each LCD page including LINE data and ID tag
void LCD_Init_Pages()
{ 
    //home page init
    LCD_Object.Page[P_HOME].Current_Line    = 0;
    LCD_Object.Page[P_HOME].Number_Of_Lines = 2;
    
    memset(LCD_Object.Page[P_HOME].Line_Text, ' ', sizeof(LCD_Object.Page[0].Line_Text[0][0]) * LCD_Object.Page[0].Number_Of_Lines * 16);
    
    strcpy(LCD_Object.Page[P_HOME].Line_Text[0], " Begin Test");
    strcpy(LCD_Object.Page[P_HOME].Line_Text[1], " Config");
    
    LCD_Object.Page[P_HOME].Cursor_On = false;
    LCD_Object.Page[P_HOME].Yes_No_Check = false;
    //test page init
    LCD_Object.Page[P_TEST].Current_Line    = 0;
    LCD_Object.Page[P_TEST].Number_Of_Lines = 2;
    
    memset(LCD_Object.Page[P_TEST].Line_Text, ' ', sizeof(LCD_Object.Page[0].Line_Text[0][0]) * LCD_Object.Page[0].Number_Of_Lines * 16);
    
    strcpy(LCD_Object.Page[P_TEST].Line_Text[0], "Leak Timer:");

    LCD_Object.Page[P_TEST].Cursor_On = true;
    LCD_Object.Page[P_TEST].Yes_No_Check = false;
    //test conferm page init
    LCD_Object.Page[P_TEST_CON].Current_Line    = 0;
    LCD_Object.Page[P_TEST_CON].Number_Of_Lines = 2;
    
    memset(LCD_Object.Page[P_TEST_CON].Line_Text, ' ', sizeof(LCD_Object.Page[0].Line_Text[0][0]) * LCD_Object.Page[0].Number_Of_Lines * 16);
    
    strcpy(LCD_Object.Page[P_TEST_CON].Line_Text[0], "Confirm Time:   ");

    LCD_Object.Page[P_TEST_CON].Cursor_On = false;
    LCD_Object.Page[P_TEST_CON].Yes_No_Check = true;
    //test conferm page init
    LCD_Object.Page[P_TEST_TIMER].Current_Line    = 0;
    LCD_Object.Page[P_TEST_TIMER].Number_Of_Lines = 2;
    
    memset(LCD_Object.Page[P_TEST_TIMER].Line_Text, ' ', sizeof(LCD_Object.Page[0].Line_Text[0][0]) * LCD_Object.Page[0].Number_Of_Lines * 16);
    
    strcpy(LCD_Object.Page[P_TEST_TIMER].Line_Text[0], "Time Left:");

    LCD_Object.Page[P_TEST_TIMER].Cursor_On = false;
    LCD_Object.Page[P_TEST_TIMER].Yes_No_Check = false;
    //test cancle page init
    LCD_Object.Page[P_CANCLE_TEST].Current_Line    = 0;
    LCD_Object.Page[P_CANCLE_TEST].Number_Of_Lines = 2;
    
    memset(LCD_Object.Page[P_CANCLE_TEST].Line_Text, ' ', sizeof(LCD_Object.Page[0].Line_Text[0][0]) * LCD_Object.Page[0].Number_Of_Lines * 16);
    
    strcpy(LCD_Object.Page[P_CANCLE_TEST].Line_Text[0], "Cancel Timer:   ");

    LCD_Object.Page[P_CANCLE_TEST].Cursor_On = false;
    LCD_Object.Page[P_CANCLE_TEST].Yes_No_Check = true;
    
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
    LCD_Object.Test_length          = 0;
    
    LCD_Object.Time_select          = 0;
    LCD_Object.Confirm              = false;
    //may or may not work needs testing
    LCD_Object.Number_Of_Pages  = sizeof(LCD_Object.Page)/sizeof(LCD_Object.Page[0]);
    //alternative
    LCD_Object.Number_Of_Pages  = LCD_PAGE_COUNT;
    //setup each LCD page 
    LCD_Init_Pages();
    
    //set the LCD in 4 bit mode
    LCD_Reset();
            
    LCD_cmd(0x06); //I/D high; SH low
    TIMER5_BlockOut(5000); //wait for cmd to be set (this is not in the function so LCD_cmd can be used with the ISR)
    LCD_cmd(0x0E); //Display on; Cursor on; Cursor Blink Off
    TIMER5_BlockOut(5000); //wait for cmd to be set
    LCD_cmd(0x28); //4 bit bus;2 line display;5x8 dot matrix
    TIMER5_BlockOut(5000); //wait for cmd to be set
    
    LCD_Clear(); //clear LCD screen
            
    LCD_state = M_IDLE; //set master status to idle
    
    LCD_Set_Page(P_HOME); //set the home page  
}
void LCD_Write_Yes_No(bool YES)
{
    //set curcer to correct time select
    LCD_cmd(0x80 + 13); 
    
    if(YES)
    {
        LCD_data('Y');
        LCD_data('e');
        LCD_data('s');
        LCD_Object.Confirm = true; 
    }
    else
    {
        LCD_data(' ');
        LCD_data('N');
        LCD_data('o');
        LCD_Object.Confirm = false;
    }                 
}
//set page do requested page
void LCD_Set_Page(LCD_PAGE_ID Page)
{
    LCD_Object.Desired_Page = Page;
    LCD_state               = M_SET_PAGE;
    
    //check current page = set page
    if(LCD_Object.Desired_Page == LCD_Object.Current_Page)
    {
        LCD_state = M_IDLE;
        if(LCD_Object.i2c_line_open == true) {LCD_Close_Coms();};
        return;
    }
    //check i2c has opened the line with the LCD
    if(LCD_Object.i2c_line_open == false) {LCD_Open_Coms();};
    //set page wanted
    LCD_Object.Current_Page = LCD_Object.Desired_Page;
    //reset cursor position and line values
    LCD_Object.Screen.Cursor_X = 0;
    LCD_Object.Screen.Cursor_Y = 0;
    LCD_Object.Page[LCD_Object.Desired_Page].Current_Line = 0;
    //clear screen this has a callback function
    LCD_Clear();
    //write current page to LCD screen
    LCD_Screen_Write();
    //default curesor dissabled
    LCD_Set_Cursor(LCD_Object.Page[LCD_Object.Desired_Page].Cursor_On);
    //check if yes no test is needed
    if(LCD_Object.Page[LCD_Object.Desired_Page].Yes_No_Check)
        LCD_Write_Yes_No(true);

    if(Page == P_HOME)
    {
        LCD_Write_Arrow(0,1);
    }
}

void LCD_Set_Cursor_Line(BYTE line)
{
    switch (line)
    {
        case 1:
            //set curcer to first line
            LCD_cmd(0x80);  
            LCD_Object.Screen.Cursor_X = 0;
            LCD_Object.Screen.Cursor_Y = 0;
            break;
        case 2:
            //set curcer to second line
            LCD_cmd(0xC0);
            LCD_Object.Screen.Cursor_X = 0;
            LCD_Object.Screen.Cursor_Y = 1;
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
            }
            break;
        case 1:
            if(LCD_Object.Page[LCD_Object.Current_Page].Current_Line + 2 != LCD_Object.Page[LCD_Object.Current_Page].Number_Of_Lines)
            {
                LCD_Object.Page[LCD_Object.Current_Page].Current_Line++;
            }
            break;
    }
    //reset cursor position
    LCD_Object.Screen.Cursor_X = 0;
    LCD_Object.Screen.Cursor_Y = 0;
    //clear screen this has a callback function
    LCD_Clear();
    
    LCD_Screen_Write();
}

void LCD_Screen_Write()
{
    LCD_state = M_SCREEN_WRITE;
    
    LCD_Close_Coms();
    LCD_Open_Coms();
    
    while(LCD_state != M_IDLE)
    {
        //write the value for the current cursor position and current line
        LCD_data(LCD_Object.Page[LCD_Object.Current_Page].Line_Text[LCD_Object.Screen.Cursor_Y + LCD_Object.Page[LCD_Object.Current_Page].Current_Line][LCD_Object.Screen.Cursor_X]);
        //increment cursor if still on screen or change line else end callback
        if      (LCD_Object.Screen.Cursor_X < 15)
            LCD_Object.Screen.Cursor_X++;
        else if (LCD_Object.Screen.Cursor_Y < 1)
            LCD_Set_Cursor_Line(2);
        else
        {
            LCD_state = M_IDLE;
        }
    }
}

void LCD_Write_Arrow(BYTE x, BYTE line)
{
    switch (line)
    {
        case 1:
            //set cursor to second line and clear current cursor
            LCD_cmd(0xC0); 
            LCD_data(' ');
            //set curcer to first line
            LCD_cmd(0x80 + x);  
            LCD_Object.Screen.Cursor_X = x;
            LCD_Object.Screen.Cursor_Y = line;
            break;
        case 2:
            //set cursor to first line and clear current cursor
            LCD_cmd(0x80); 
            LCD_data(' ');
            //set curcer to second line
            LCD_cmd(0xC0 + x);
            LCD_Object.Screen.Cursor_X = x;
            LCD_Object.Screen.Cursor_Y = line;
            break;
    }
    //125 is the decimal value for a right arrow
    LCD_data(125);
}

inline void LCD_Dectoment_Timer()
{
    LCD_Object.Test_length -= 2;
}

void LCD_Manager(BYTE Command)
{
    //check i2c has opened the line with the LCD
    if(LCD_Object.i2c_line_open == false) {LCD_Open_Coms();};
    //witch between LCD states
    switch(LCD_Object.Current_Page)
    {
        case P_HOME:
            //if command is a click
            switch(Command)
            {
                case 0:
                    //find witch option is solected then do action accosiated
                    switch(LCD_Object.Screen.Cursor_Y)
                    {
                        case 1:
                            LCD_Set_Page(P_TEST);
                            LCD_Set_Cursor_Line(2);
                            LCD_Write_Test_Time();
                            break;
                        case 2:
                            
                            break;          
                    }
                    break;
                case 1:
                    LCD_Write_Arrow(0, 1);
                    break;
                case 2:
                    LCD_Write_Arrow(0, 2);
                    break;
            }            
            break;
        case P_TEST:
            //if command is a click
            switch(Command)
            {
                case 0:
                    if(LCD_Object.Time_select < 2)
                    {
                        LCD_Object.Time_select++;
                        //set curcer to correct time select
                        LCD_cmd(0xC0 + 1 + (LCD_Object.Time_select * 3));
                    }
                    else 
                    {
                        LCD_Set_Page(P_TEST_CON);
                        LCD_Set_Cursor_Line(2);
                        LCD_Write_Test_Time();
                        //reset time corser
                        LCD_Object.Time_select = 0;
                    }              
                    break;
                    
                case 1:
                    switch(LCD_Object.Time_select)
                    {
                        case 0:
                            LCD_Object.Test_length += 3600;
                            break;
                        case 1:
                            LCD_Object.Test_length += 60;
                            break;
                        case 2:
                            LCD_Object.Test_length += 1;
                            break;
                    }
                    if(LCD_Object.Test_length > 28800)
                        LCD_Object.Test_length -= (LCD_Object.Test_length/3600)*3600;
                    //update lcd time
                    LCD_Write_Test_Time();
                    break;
                    
                case 2:
                    switch(LCD_Object.Time_select)
                    {
                        case 0:
                            LCD_Object.Test_length -= 3600;
                            break;
                        case 1:
                            LCD_Object.Test_length -= 60;
                            break;
                        case 2:
                            LCD_Object.Test_length -= 1;
                            break;
                    }
                    if(LCD_Object.Test_length < 0)
                        LCD_Object.Test_length += 28800;
                    //update lcd time
                    LCD_Write_Test_Time();
                    break;
            }
            break;
        case P_TEST_CON:
            switch(Command)
            {
                case 0: //button press
                    switch(LCD_Object.Confirm)
                    {
                        case true:
                            LCD_Set_Page(P_TEST_TIMER);
                            LCD_Set_Cursor_Line(2);
                            LCD_Write_Test_Time();
                            LCD_Object.Stored_Test_Length = LCD_Object.Test_length;
                            //Get_Vac_Start();
                            Start_Timer();
                            break;
                        case false:
                            LCD_Set_Page(P_TEST);
                            LCD_Set_Cursor_Line(2);
                            LCD_Write_Test_Time();
                            break;
                    }
                    break;
                case 1: //dial up
               
                case 2: //dial down 
                    LCD_Write_Yes_No(!LCD_Object.Confirm);                
                    break;
            }
            break;
        case P_TEST_TIMER:
            switch(Command)
            {
                case 0:
                    LCD_Set_Page(P_CANCLE_TEST);
                    break;
                case 3: //timer callback
                    //decroment time and check if its less then 0
                    if(LCD_Object.Test_length > 0)
                    {
                        LCD_Set_Cursor_Line(2);
                        LCD_Write_Test_Time();
                        break;
                    }
                    else
                    {
                        LCD_Set_Page(P_HOME);
                    }
            }
            break;
        case P_CANCLE_TEST:
            switch(Command)
            {
                case 0: //button press
                    switch(LCD_Object.Confirm)
                    {
                        case true:
                            LCD_Set_Page(P_HOME);
                            break;
                        case false:
                            LCD_Set_Page(P_TEST_TIMER);
                            LCD_Set_Cursor_Line(2);
                            LCD_Write_Test_Time();
                            break;
                    }
                    break;
                case 1: //dial up
               
                case 2: //dial down
                    //toggle yes/no
                    LCD_Write_Yes_No(!LCD_Object.Confirm);                 
                    break;
            }
            break;
    }
    
    //check i2c has closed the line with the LCD
    if(LCD_Object.i2c_line_open == true) {LCD_Close_Coms();};
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