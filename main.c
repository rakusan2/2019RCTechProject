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

void wifi_recieve(unsigned char *data){
    
}

/*
 * 
 */
int main(int argc, char** argv) {
    wifi_init();

    return (EXIT_SUCCESS);
}
