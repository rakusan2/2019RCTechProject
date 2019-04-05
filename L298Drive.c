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

#define OC5R_PER_BIT 2

int drive_curSpeed = 0;
int drive_speed = 0;
int drive_rate = 0x2;
int drive_negLimit = -255;
int drive_posLimit = 255;

void drive_setCurSpeed(int speed){
    drive_curSpeed = speed;
    if(speed < 0){
        PORTB = (PORTB & 0xf3ff) | 0x800;
        speed = -speed;
    }else if(speed == 0){
        PORTACLR = 3;
    }else{
        PORTB = (PORTB & 0xf3ff) | 0x400;
    }
    OC5RS = speed * OC5R_PER_BIT;
}

void drive_set(int speed){
    drive_speed = bind(-255, speed, 255);
}

void drive_pwmRefresh(){
    if(drive_speed != drive_curSpeed && isBetween(drive_negLimit, drive_curSpeed, drive_posLimit)){
        int speed = drive_curSpeed + (drive_curSpeed < drive_speed ? drive_rate : -drive_rate);
        steer_setCurSpeed(bind(drive_negLimit, speed, drive_posLimit));
    }
}

void drive_setMax(int max){
    drive_posLimit = bind(0, max, 255);
}

signed int getMotorSet(unchar *data){
    unchar ch = data[0];
    if(ch == 'p'){
        return 0x3f;
    }else if(ch == 'n'){
        return -0x3f;
    }else if(ch == '\\'){
        return data[1];
    }else if(ch == '-' || (ch >= '0' && ch <= '9')){
        signed int num = 0;
        uint index = 0;
        signed int sign = 1;
        if(ch == '-'){
            index = 1;
            sign = -1;
        }
        ch = data[index];
        while((ch >= '0' && ch <= '9')){
            num = (num * 10) + ch - '0';
            index++;
            ch = data[index];
        }
        return num * sign;
    }else{
        return 0;
    }
}
void drive_deserializer(unchar *data, uint len){
    if(len>0){                                                                                                                                                                                 
        drive_set(getMotorSet(data));
    }
    se_addStr_("D_set=");
    se_addNum(drive_speed);
    se_addStr_(",D_cur=");
    se_addNum(drive_curSpeed);
}
void drive_init(){
    
    TRISACLR = 0x3;
    
    RPA4R = 0b0110;     // Set Port A4 to use OC5
    OC5CON = 0x0006;    // Enable PWM from Timer 2
    
    OC5CONSET = 0x8000; 
    drive_setCurSpeed(0);
}