/* 
 * File:   main.c
 * Author: Rakus
 *
 * Created on January 16, 2019, 10:31 AM
 */

#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>
#include <stdlib.h>
#include "wifi.h"
#include "tools.h"
#include "sonic.h"
#include "turnSensor.h"
#include "deserializer.h"
#include "serializer.h"
#include "I2CMaster.h"
#include "MPU6050.h"
#include "battery.h"
#include "L298Steer.h"


// USERID = No Setting
#pragma config PMDL1WAY = ON            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow only one reconfiguration)

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
#pragma config ICESEL = ICS_PGx3        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF   

struct USER{
    unchar connected;
    unchar nl;
    unchar *repeatCMD;
    unchar repeatTime;
} users[5];

/**
 * The interpreter for the Data recieved from the WiFi
 * @param data  The received bytes
 * @param len   The Amount of the bytes received
 */
void wifi_receive(unchar *data, uint len) {
    if (startsWith(data, len, "+IPD", 4)) { // TCP Data is structured as (+IPD,0,n:xxxxxxxxxx)	
        unchar userID = data[5] - '0';
        uint collon = 6;
        while (data[collon] != ':') {
            collon++;
            if(collon >= len){
                return;
            }
        }
        dese_deserialize(userID ,data + collon + 1, len - (collon + 1));
        se_noEmpty();
        se_sendToWifi(userID);
        se_clear();
    }
    
    unchar firstCh = data[0];
    unchar isID = isBetween('0',firctCh, '4');
    
    if(isID && startsWith(data + 1,len, ",CON",4)){
        users[firstCh - '0'].connected = 1;
    }else if(isID && startsWith(data + 1,len, ",DIS",4)){
        users[firstCh - '0'].connected = 0;
    }
    if(data[0] >= '0' && data[0] <= '9' && data[1] == ','){
        _nop();
    }
}


/**
 * Exception Handler
 * @param cause     The CPU Cause register
 * @param status    The CPU Status Register
 */
void _general_exception_handler (unsigned cause, unsigned status){
    _nop();     // Something to stop on during DEBUG
}

int startCounter = 0;   // Main Loop run count

/**
 * The Main Loop
 */
void __ISR(_TIMER_1_VECTOR, IPL1SOFT) mainLoop(){
    mpu_refresh();  // Refresh the MPU
    bat_convert();  // Measure the battery voltage
    
    startCounter++;
    if(startCounter == 50){
        wifi_forceStart(); // After 2s try to force start the WiFi
    }
    
    IFS0bits.T1IF = 0;
}

/**
 * The Main function
 */
int main(int argc, char** argv) {
    ANSELA = 0; // Disable All Analog pins to be used as digital
    ANSELB = 0;
    
    //initMVEC();
    INTCONSET = 0x1000; // Set MVEC bit
    __builtin_enable_interrupts();  // Enable the interrupts
    
    wifi_init();    
    sonic_init();
    ts_init();
    bat_init();
    steer_init();
    i2c_init();
    mpu_init();
    
    wifi_setSoftAP("Small Device", "pic32mx170f256b");  //Setup the access point
    wifi_startTCPServer("8888");    // Start a TCP server on port 8888
    
    T1CON = 0x20;       // Setup Main Loop Timer
    PR1 = 30000;        // Set Main Loop to run at 25Hz
    
    IEC0bits.T1IE = 1;  // Enable Timer 1 interrupt
    IPC1bits.T1IP = 1;  // Set Main Loop priority to 1
    
    T1CONSET = 0x8000;  // Start Main Loop Timer
    
    while (1) {
        _wait();
    }
    return (EXIT_SUCCESS);
}
