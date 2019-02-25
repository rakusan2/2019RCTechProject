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
#include <proc/p32mx170f256b.h>
#include "tools.h"
#include "deserializer.h"
#include "serializer.h"

#define MM_PER_64_CYCLES_X16B 14986   // 343/2 * 1000 * 64/48E6 * 2^16

int32_t sonic_distStart = 0;
int32_t sonic_dist = 0;

int32_t sonic_smallest=0;

inline void clearIC4Buf(){
    while(IC4CONbits.ICBNE){
        IC4BUF;
    }
}

void __ISR(_TIMER_2_VECTOR, IPL3SOFT) sonicTimerInt(){
    sonic_smallest = 0;
    IFS0bits.T2IF = 0;
}

void __ISR(_INPUT_CAPTURE_4_VECTOR, IPL3SOFT) sonicInt(){
    if(IFS0bits.IC4IF){
        if(IC4CONbits.ICBNE){
            if(sonic_smallest){
                sonic_smallest = IC4BUF;
                sonic_distStart = sonic_smallest * MM_PER_64_CYCLES_X16B;
                sonic_distStart >>=16;                
            }else{
                int32_t temp = IC4BUF
                sonic_dist = (temp - sonic_smallest) * MM_PER_64_CYCLES_X16B;
                sonic_smallest = temp;
                sonic_dist >>= 16;
            }
        }
        clearIC4Buf();
        IFS0bits.IC4IF = 0;
    }
}

void sonic_serializeData(unchar *data, uint len){
    if(len >0 ){
        se_addStr_("Uwidth=");
        se_addUNum(sonic_dist);
        se_addStr_(",Ustart=");
        se_addUNum(sonic_distStart);
    }else{
        se_addStr_("U=");
        se_addUNum(sonic_dist);
    }
}

void sonic_init(){
    IC4R = 0b0100;      // Set Echo to Port B7 using IC4
    RPB4R = 0b0101;     // Set Trigger to Port B4 using OC1
    
    T2CON = 0x0060;     // Set prescaler to 1:64
    PR2 = 45000;        // Set period to 60ms
    
    clearIC4Buf();
            
    IC4CON = 0x8281;    // Timer 2, Int on 2nd capture, rising then every other
    OC1CON = 0x0005;    // Generate pulses on OC1 using Timer 2
    OC1R = 1;           // Set OC1 High on the start of the timer
    OC1RS = 100;          // Set OC1 Low after 10us
    
    IEC0bits.IC4IE = 1; // Enable IC4 Interrupt
    IPC4bits.IC4IP = 3; // Set Interrupt Priority to 3
    
    IPC2bits.T2IP = 3;
    IPC2bits.T2IS = 2;
    IEC0bits.T2IE = 1;
    
    OC1CONSET = 0x8000; // Start Output Compare
    T2CONSET = 0x8000;  // Start Timer 2
}
