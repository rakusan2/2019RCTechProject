/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#include <xc.h>
#include <sys/attribs.h>
#include "tools.h"
#include "serializer.h"

#define BAT_ADC_TO_VOLT_X24 54067

int32_t bat_val=0;

inline void bat_sample(){
    AD1CON1SET = 0x0002;    // Sample 
}

inline void bat_convert(){
    AD1CON1CLR = 0x0002;
}

void __ISR(_ADC_VECTOR, IPL1SOFT) batInt(){
    bat_val = ADC1BUF0 * BAT_ADC_TO_VOLT_X24;
    bat_sample();
    IFS0bits.AD1IF = 0;
}

void bat_init(){
    AD1CON1 = 0x0400;
    AD1CON2 = 0x0000;
    AD1CON3 = 0x000f;
    AD1CHS =  0x0c0c0000;
    ANSELBbits.ANSB12 = 1;
    
    IPC5bits.AD1IP = 1;
    IPC5bits.AD1IS = 1;
    IEC0bits.AD1IE = 1;
    
    AD1CON1SET = 0x8000;    // Enable ADC
    bat_sample(); 
}


void bat_serialize(){
    se_addStr_("B=");
    se_add0824Num(bat_val, 4);
}