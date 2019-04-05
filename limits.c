/*
 * File:   turnSensor.c
 * Author: Tomas
 *
 * Created on January 20, 2019, 11:27 AM
 */


#include <xc.h>
#include <sys/attribs.h>
#include "tools.h"
#include "serializer.h"
#include "deserializer.h"
#include "L298Steer.h"

uint ts_lastState=0;

void __ISR(_CHANGE_NOTICE_VECTOR, IPL4SOFT) PortChangeInt(){
    if(IFS1bits.CNBIF){
        uint current = PORTB & 0xA;
        uint changed = ts_lastState ^ current;
        if(changed>0){
            if(changed & current & 2){  // 2nd bit (White Line) went high
                steer_trigEnd();
            }
            if(changed & ts_lastState & 8){ // 4th bit (Red Line) went low
                steer_trigCenter();
            }
            ts_lastState = current;
        }
    }
    CNSTATA = 0;
    IFS1CLR=(7<<13);
}

/**
 * Interpret the command sent to the limits
 * @param data  The Command
 * @param len   The Length of the command
 */
void ts_deserialize(unchar *data, uint len){
    se_addStr_("L=");
    se_addUNum(ts_lastState);
}

/**
 * Initialize the Limits
 */
void ts_init(){
    
    TRISBSET = 0xA;
    CNENB = 0xA;
    CNPDBSET = 0xA;
    CNCONBbits.ON = 1;
    IPC8bits.CNIP = 4;
    IEC1bits.CNBIE = 1;
    
}
