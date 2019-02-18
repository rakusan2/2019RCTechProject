/*
 * File:   turnSensor.c
 * Author: Tomas
 *
 * Created on January 20, 2019, 11:27 AM
 */


#include <xc.h>
#include <sys/attribs.h>
#include "tools.h"

uint lastState=0;

void switchChange(uint data);

//void inputChange(){
//    uint current = (CMSTAT & 0x3) + ((PORTB & 0x5)>>2);
//    if(lastState != current){
//        lastState = current;
//        switchChange(current);
//    }
//}

//void __ISR(_COMPARATOR_1_VECTOR, IPL4SOFT) COMP1Int(){
//    inputChange();
//    IFS1bits.CMP1IF=0;
//}
//void __ISR(_COMPARATOR_2_VECTOR, IPL4SOFT) COMP2Int(){
//    inputChange();
//    IFS1bits.CMP2IF=0;
//}
void __ISR(_CHANGE_NOTICE_VECTOR, IPL4SOFT) PortChangeInt(){
    if(IFS1bits.CNBIF){
        uint current = PORTB & 0xF;
        if(lastState != current){
            lastState = current;
            switchChange(current);
        }
    }
    IFS1CLR=(7<<13);
}

void ts_init(){
    // Due to lines Blue and Green Having 1.6V when ON, Comparator 1 and 2 need to be used
    
//    CM1CON = 0x8053;
//    CM2CON = 0x8053;
//    IEC1bits.CMP1IE=1;
//    IEC1bits.CMP2IE=1;
//    IPC6bits.CMP1IP=4;
//    IPC7bits.CMP2IP=4;
    
    TRISBSET = 0xF;
    IEC1bits.CNBIE = 1;
}