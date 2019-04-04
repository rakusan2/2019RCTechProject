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

void drive_init(){
    
    TRISACLR = 0x3;
    
    RPA4R = 0b0110;     // Set Port A4 to use OC5
    OC5CON = 0x0006;    // Enable PWM from Timer 2
    
    OC5CONSET = 0x8000; 
    drive_setCurSpeed(0);
}