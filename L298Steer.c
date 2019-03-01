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
#include "SPIHBridge.h"

#define OC4R_PER_BIT 94

int steer_speed=0;

void steer_set(int speed){
    steer_speed = speed;
    if(speed < 0){
        PORTA = (PORTA & 0xfffc) | 0x2;
        speed = -speed;
    }else{
        PORTA = (PORTA & 0xfffc) | 0x1;
    }
    if(speed == 0 || speed >= 0xff){
        OC4CONCLR = 0x8000;
        PORTBbits.RB0 = (speed != 0);
    }else{
        OC4CONSET = 0x8000;
        OC4R = speed * OC4R_PER_BIT;
    }
}

void steer_deserializer(unchar *data, uint len){
    if(len>0){
        steer_set(getMotorSet(data));
    }
    se_addStr_("S=");
    se_addNum(steer_speed);
}

void steer_init(){
    TRISACLR = 0x3;
    
    RPB0R = 0b0101;     // Set Port B0 to use OC3
    OC3CON = 0x0006;    // Enable PWM from Timer 2
    
    PR2 = 24000;
    T2CON = 0x8060;
    
    steer_set(0);
}

// PR2 = 0x7f