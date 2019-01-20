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


void wifi_receive(unchar *data, uint len) {
    if (startsWith(data, len, "+IPD", 4)) {
        unchar echo[10];
        len = min(16, len);
        uint i;
        for (i = 6; i < len; i++) {
            echo[i - 6] = data[i];
        }
        wifi_send(echo, len - 6);
    }
}


/*
 * 
 */
int main(int argc, char** argv) {
    wifi_init();
    wifi_setSoftAP("Small Device","pic32mx170f256b");
    wifi_startTCPServer("8888");
    while (1) {
        _wait();
    }
    return (EXIT_SUCCESS);
}
