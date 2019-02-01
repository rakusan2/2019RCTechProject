/* 
 * File:   main.c
 * Author: Rakus
 *
 * Created on January 16, 2019, 10:31 AM
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "wifi.h"
#include "tools.h"
//#include "turnSensor.h"
//#include "I2CMaster.h"
//#include "MPU6050.h"

// USERID = No Setting
#pragma config PMDL1WAY = OFF            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
#pragma config IOL1WAY = OFF             // Peripheral Pin Select Configuration (Allow only one reconfiguration)

// DEVCFG2
#pragma config FPLLIDIV = DIV_1        // PLL Input Divider (12x Divider)
#pragma config FPLLMUL = MUL_24         // PLL Multiplier (24x Multiplier)
#pragma config FPLLODIV = DIV_2       // System PLL Output Clock Divider (PLL Divide by 256)

// DEVCFG1
#pragma config FNOSC = FRCPLL              // Oscillator Selection Bits (Primary Osc (XT,HS,EC))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = OFF            // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config JTAGEN = OFF              // JTAG Enable (JTAG Port Enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

unchar txData[1000];
uint txLen = 0;

void txDataAdd(unchar *data, uint len) {
    uint i;
    for (i = 0; i < len; i++) {
        txData[txLen] = data[i];
        txLen++;
    }
}

void txDataAdd_(unchar *data) {
    int i;
    for (i = 0; data[i] != 0; i++) {
        txData[txLen] = data[i];
        txLen++;
    }
}

inline uint isMotorSet(unchar c) {
    return (c >= '0' && c <= '9') || c == '+' || c == '-' || c == 'P' || c == 'N' || c == '\\';
}

signed int getMotorSet(uint *start, unchar *data) {
    uint index = *start;
    unchar c = data[index];
    if (c == 'P') {
        *start++;
        return 0xff;
    } else if (c == 'N') {
        *start++;
        return -0xff;
    } else if (c == '\\') {
        *start += 2;
        uint num = data[index + 1] << 1;
        return num > 0xff ? num & 0xff : num;
    } else {
        uint pos = 0x100;
        uint num = 0;
        if (c == '+') {
            index++;
            c = data[index];
        } else if (c == '-') {
            pos = 0;
            index++;
            c = data[index];
        }
        uint count = 0;
        while ((c >= '0' || c <= '9') && count < 3) {
            count++;
            num = (num * 10) + c - '0';
            index++;
            c = data[index];
        }
        *start = index;
        return pos ? num : -num;
    }
}

inline int isMotionSpecifier(unchar c) {
    return c == 'X' || c == 'Y' || c == 'Z';
}

void readTCPStart(uint index, unchar *data, uint len) {
    while (index < len) {

        switch (data[index]) {
            case 'D':
                index++;
                if (isMotorSet(data[index])) {
                    signed int set = getMotorSet(&index, data);

                }
                txDataAdd_("D");
                // Drive instruction
                break;
            case 'S':
                index++;
                if (isMotorSet(data[index])) {
                    uint set = getMotorSet(&index, data);

                }
                txDataAdd_("S");
                // Steer Instruction
                break;
            case 'A':
                index++;
                if (isMotionSpecifier(data[index])) {

                    index++;
                }
                txDataAdd_("A");
                // Accelerometer Instruction
                break;
            case 'G':
                index++;
                if (isMotionSpecifier(data[index])) {

                    index++;
                }
                txDataAdd_("G");
                // Gyro Instruction
                break;
            case 'L':
                index++;
                txDataAdd_("L");
                // Limit switches instruction
                break;
            case 'U':
                index++;
                txDataAdd_("U");
                // Ultrasound Instruction
                break;
            case 'V':
                index++;
                txDataAdd_("V");
                // Version Instruction
                break;
            default:
                return;
        }
        if(index<len){
            txDataAdd_(",");
        }
    }
}

void wifi_receive(unchar *data, uint len) {
    if (startsWith(data, len, "+IPD", 4)) { // TCP Data is structured as (+IPD,0,n:xxxxxxxxxx)	
        uint collon = 7;
        while (data[collon] != ':') {
            collon++;
        }
        readTCPStart(collon + 1, data, len);
    }
}

void switchChange(uint data) {

}

void wait(uint clocks) {
    while (clocks > 0) {
        clocks++;
    }
}

/*
 * 
void switchChange

 */
int main(int argc, char** argv) {
    ANSELA = 0; // Disable All Analog pins to be used as digital
    ANSELB = 0;
    INTCONbits.MVEC = 1; // Enable Multi Vector Interrupts
    asm volatile("ei"); // Enable Interrupts

    wifi_init();
    //ts_init();
    //i2c_init();
    //mpu_init();
    wifi_setSoftAP("Small Device", "pic32mx170f256b");
    wifi_startTCPServer("8888");
    while (1) {
        _wait();
    }
    return (EXIT_SUCCESS);
}
