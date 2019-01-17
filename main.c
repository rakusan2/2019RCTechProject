/* 
 * File:   main.c
 * Author: Rakus
 *
 * Created on January 16, 2019, 10:31 AM
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/attribs.h>

// DEVCFG0
#pragma config CP = OFF
#pragma config PWP = OFF

// DEVCFG1
#pragma config FWDTWINSZ = 0 //Watchdog window size to 25%
#pragma config FWDTEN = 0 // Disable Watchdog
#pragma config WINDIS = 1 // Watchdog is not windowed
#pragma config FCKSM = CSDCMD
#pragma config FPBDIV = 0 // Peripheral bus clock devided by 1
#pragma config OSCIOFNC = 1
#pragma config POSCMOD = 0b00 //Use external
#pragma config IESO = 1
#pragma config FSOSCEN = OFF
#pragma config FNOSC = 0b010 // Primary oscillator

//DEVCFG3
#pragma config IOL1WAY = 0
#pragma config PMDL1WAY = 0



/*
 * 
 */
int main(int argc, char** argv) {
    U1RXR = 0x0011; // Set UART1 RX to Port B13
    RPB15R = 0b0001; // Set UART1 TX to Port B15



    U1MODE = 0b1000100010000000;
    U1STA = 0x1400;
    U1BRG = 0x0019;
    U1MODESET = 0x8000;
    

    return (EXIT_SUCCESS);
}

void __ISR()