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



void wifi_receive(unchar *data, uint len) {
    if (startsWith(data, len, "+IPD", 4)) { // TCP Data is structured as (+IPD,0,n:xxxxxxxxxx)	
        unchar echo[10];
        len = min(16, len);
        uint i;
        for (i = 6; i < len; i++) {
            echo[i - 6] = data[i];
        }
        wifi_send(echo, len - 6, data[5]);
    }
}

void switchChange(uint data){
    
}

 void wait(uint clocks){
    while(clocks>0){
        clocks++;
    }
}

/*
 * 
void switchChange

 */
int main(int argc, char** argv) {
    ANSELA = 0;     // Disable All Analog pins to be used as digital
    ANSELB = 0;
    INTCONbits.MVEC=1;  // Enable Multi Vector Interrupts
    asm volatile("ei"); // Enable Interrupts
   
    wifi_init();
    //ts_init();
    //i2c_init();
    //mpu_init();
    wifi_setSoftAP("Small Device","pic32mx170f256b");
    wifi_startTCPServer("8888");
    while (1) {
        _wait();
    }
    return (EXIT_SUCCESS);
}
