/*
 * File:   DAC_ADC.c
 * Author: Jacob7395
 *
 * Created on 08 September 2016, 14:43
 */


#include "DAC_ADC.h"
#include "LCD.h"

#include <math.h>

float ADC_One_Get_Val()
{
    //setup regeisters
    ADCON0 = 0b00010001;
    ADCON1 = 0b11010000;
    ADCON2 = 0b00000000;
    //start convertion
    ADCON0bits.GO = 1;
    //wait till convertion finisher
    while(ADCON0bits.GO_nDONE == 1) {};
    //convert vale into float
    int max = 0x3FF;
    unsigned int val = ((ADRESH & 0b11) << 8) + ADRESL;
    //calculate voltage and scale with max volts
    float volts = ((float)val/(float)max) * 5;
    
    return volts;
}

void Set_DAC(int volts)
{
    //wait for DAC to be ready for new value
    while(DACLDbits.DAC1LD) {};
    //set pin to output
    TRISAbits.TRISA0 = 1;
    //setup DAC configs
    DAC1CON0 = 0b10100000;    
    //set DAC refrences
    DAC1REFL = 0x00ff &  volts;
    DAC1REFH = 0x00ff & (volts >> 8);
    
    DACLDbits.DAC1LD = 1;    
}

void Get_Vac_Start()
{   
    float vac = pow(10,(1.66628 * (ADC_One_Get_Val() * 2) - 11.33));
    //problem passing floats this allows
    Load_Start_Vac(vac);
}

void Get_Vac_End()
{   
    float vac = pow(10,(1.66628 * (ADC_One_Get_Val() * 2) - 11.33));
    //problem passing floats this allows loading the float to LCD array
    Load_End_Vac(vac);
}
