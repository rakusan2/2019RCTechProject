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

#define OC4R_PER_BIT 2

int steer_speed = 0;
int steer_curSpeed =0;

int steer_posLimit = 256;
int steer_negLimit = -256;

int steer_rate = 0x2;
int steer_aim =0;

void steer_setCurSpeed(int speed){
    steer_curSpeed = speed;
    if(speed < 0){
        PORTA = (PORTA & 0xfffc) | 0x2;
        speed = -speed;
    }else if(speed == 0){
        PORTACLR = 3;
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

void steer_set(int speed){
    steer_speed = max(min(speed,256),-256);
}

void __ISR(_TIMER_2_VECTOR, IPL6SOFT) PWMInt(){
    if(steer_speed != steer_curSpeed){
        if(steer_speed > steer_negLimit && steer_curSpeed < steer_posLimit){
            int speed = steer_curSpeed + (steer_curSpeed < steer_speed ? steer_rate : -steer_rate);
            steer_setCurSpeed(max(min(speed,steer_posLimit),steer_negLimit));
        }
    }
    IFS0bits.T2IF = 0;
}

void steer_setPosLimit(int speed){
    steer_posLimit = max(min(speed,256),0);
    if(steer_curSpeed > steer_posLimit){
        steer_setCurSpeed(steer_posLimit);
    }
}

void steer_setNegLimit(int speed){
    steer_negLimit = max(min(speed,0),-256);
    if(steer_curSpeed < steer_negLimit){
        steer_setCurSpeed(steer_negLimit);
    }
}

void steer_trigEnd(){
    steer_setCurSpeed(0);
    steer_speed = 0;
}
void steer_trigCenter(){
    if(steer_aim != 0){
        steer_set(0);
    }
}

void steer_deserializer(unchar *data, uint len){
    if(len>0){
        if(data[0] == 'l'){
            steer_aim = -1;
            data++;
            len--;
            if(len == 0){
                steer_set(-126);
            }
        }else if(data[0] == 'r'){
            steer_aim = 1;
            data++;
            len--;
            if(len == 0){
                steer_set(126);
            }
        }else{
            if(data[0] == 'c'){
                data++;
                len--;
                if(len==0){
                    steer_set(steer_aim < 0 ? 126 : -126);
                }
            }
            steer_aim = 0;
        }
        if(len>0){                                                                                                                                                                                    
            steer_set(getMotorSet(data));
        }
    }
    se_addStr_("S_set=");
    se_addNum(steer_speed);
    se_addStr_(",S_cur=");
    se_addNum(steer_curSpeed);
}

void steer_init(){
    TRISACLR = 0x3;
    
    RPB0R = 0b0101;     // Set Port B0 to use OC3
    OC3CON = 0x0006;    // Enable PWM from Timer 2
    
    PR2 = 512;
    T2CON = 0x8060;
    
    IPC2bits.T2IP = 6;
    IPC2bits.T2IS = 2;
    IEC0bits.T2IE = 1;
    
    steer_set(0);
}

// PR2 = 0x7f