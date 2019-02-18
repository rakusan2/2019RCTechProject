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

#define UM_PER_64_CYCLES_X16B 14986   // 343/2 * 1E6 * 64/48E6 * 2E16

int32 sonic_distStart = 0;
int32 sonic_dist = 0;

void __ISR(_INPUT_CAPTURE_4_VECTOR, IPL3SOFT) sonicInt(){
    if(IFS0bits.IC4IF){
        if(IC4CONbits.ICBNE){
            sonic_distStart = IC4BUF;
            sonic_dist = (IC4BUF - sonic_distStart) * UM_PER_64_CYCLES_X16B;
            sonic_distStart *= UM_PER_64_CYCLES_X16B;
            sonic_dist >>= 16;
            sonic_distStart >>=16;
        }
        IFS0bits.IC4IF = 0;
    }
}

void sonic_init(){
    IC4R = 0b0100;      // Set Echo to Port B7 using IC4
    RPB4R = 0b0101;     // Set Trigger to Port B4 using OC1
    
    T2CON = 0x0020;     // Set prescaler to 1:64
    PR2 = 45000;        // Set period to 60ms
    
    IC4CON = 0x82A6;    // Timer 2, Int on 2nd capture, rising then every other
    OC1CON = 0x0005;    // Generate pulses on OC1 using Timer 2
    OC1R = 0;           // Set OC1 High on the start of the timer
    OC1RS = 8;          // Set OC1 Low after 10us
    
    IEC0bits.IC4IE = 1; // Enable IC4 Interrupt
    IPC4bits.IC4IP = 3; // Set Interrupt Priority to 3
    
    OC1CONSET = 0x8000; // Start Output Compare
    T2CONSET = 0x8000;  // Start Timer 2
}