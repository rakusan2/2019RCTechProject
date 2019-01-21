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

#include <xc.h>
#include <sys/attribs.h>
#include <stdlib.h>
#include "tools.h"

#define UART1_PRIORITY 6
#define TXBufSize 2000
#define RXBufSize 2000

unchar txBuf[TXBufSize];
unchar rxBuf[RXBufSize];
uint txBufLen = 0;
uint rxBufLen = 0;
uint txPointer = 0;
uint rxPointer = 0;

int pauseTX=0;
// Located In Main
void wifi_receive(unsigned char *data, unsigned int len);

/**
 * Try to add a byte to the TX buffer
 */
void __sendNext() {
    if (!U1STAbits.UTXBF && txPointer < txBufLen && !pauseTX) {
        U1TXREG = txBuf[txPointer];
        txPointer++;
        if (txPointer == txBufLen) {
            txPointer = txBufLen = 0;
        }
        if(txBuf[txPointer]=='\n'){
            pauseTX=1;
        }
        __sendNext();
    }
}

/**
 * Test received data for whether transmission should resume or reset
 * 
 * @param data  Received data
 * @param len   Length of received data
 */
void resumeTX(unchar *data,uint len){
    if(startsWith(data,len,"FAIL",2)){
        txBufLen = txPointer = 0;
    }else{
        pauseTX=0;
    }
    wifi_receive(data,len);
}

/**
 * Add a character to the transmission buffer
 * 
 * @param c character to be added
 */
inline void txBufAddChar(unchar c){
    txBuf[txBufLen] = c;
    txBufLen++;
}

/**
 * Add an Array of characters to the TX buffer
 * 
 * @param data  Array of characters
 * @param len   Length of the array
 */
void txBufAdd(unchar *data, uint len) {
    uint i;
    for (i = 0; i < len; i++) {
        txBuf[txBufLen + i] = data[i];
    }
    txBufLen += len;
}

/**
 * Add a null terminated Array of characters to the buffer
 * 
 * @param data  Array of characters
 */
void txBufAdd_(unchar *data){
    uint i=0;
    while(data[i] !='\0'){
        txBuf[txBufLen + i] = data[i];
        i++;
    }
    txBufLen+=i;
}

/**
 * Add an Array of characters to the TX buffer surrounded by quotations
 * 
 * @param data  Array of characters
 * @param len   Length of the array
 */
inline void txBufAddStr(unchar *data, uint len){
    txBufAddChar('"');
    txBufAdd(data,len);
    txBufAddChar('"');
}

/**
 * Add a null terminated Array of characters to the TX buffer surrounded by quotations
 * 
 * @param data  Array of characters
 */
inline void txBufAddStr_(unchar *data){
    txBufAddChar('"');
    txBufAdd_(data);
    txBufAddChar('"');
}

/**
 * Add an Array of characters to the TX buffer followed by a new line
 * 
 * @param data  Array of characters
 * @param len   Length of the array
 */
inline void txBufAddLn(unchar *data, uint len) {
    txBufAdd(data, len);
    txBufAdd("\r\n", 2);
    __sendNext();
}

/**
 * Add a null terminated Array of characters to the TX buffer followed by a new line
 * 
 * @param data  Array of characters
 */
inline void txBufAddLn_(unchar *data) {
    txBufAdd_(data);
    txBufAdd("\r\n", 2);
    __sendNext();
}

/**
 * UART Interrupt handler
 */
void __ISR(_UART_1_VECTOR, IPL6SOFT) UARTInt() {
    if (IFS1bits.U1RXIF) {
        while (U1STAbits.URXDA) { // While there are data in the receive buffer
            rxBuf[rxPointer] = U1RXREG;
            if(pauseTX && rxBuf[rxPointer] == '>'){ // Resume when READY to transmit TCP DATA
                pauseTX = 0;
            }
            if (rxBuf[rxPointer] == '\n') {
                resumeTX(rxBuf, rxPointer + 1);
                rxPointer = 0;
            } else {
                rxPointer++;
            }
        }
        IFS1bits.U1RXIF = 0;
    }
    if (IFS1bits.U1TXIF) { // Add new data to the TX buffer after a transmission has happened
        __sendNext();
        IFS1bits.U1TXIF = 0;
    }
}

/**
 * Initialize the UART for the WiFi module
 */
void wifi_init() {
    int i;
    for (i = 0; i < TXBufSize; i++) {
        txBuf[i] = 0;
    }
    for (i = 0; i < RXBufSize; i++) {
        rxBuf[i] = 0;
    }

    U1RXR = 0x0011; // Set UART1 RX to Port B13
    RPB15R = 0b0001; // Set UART1 TX to Port B15

    U1MODE = 0x8880;
    U1STA = 0x1400;
    U1BRG = 0x0019;

    IEC1bits.U1RXIE = 1; // Enable Receive Interrupt
    IEC1bits.U1TXIE = 1; // Enable Transmit Interrupt
    IPC8bits.U1IP = UART1_PRIORITY; // Set Priority Level
}

/**
 * Send TCP data
 * 
 * @param data      Array of characters to be sent
 * @param len       Length of the Array
 * @param linkID    Recipient ID
 */
void wifi_send(unchar *data, uint len, unchar linkID) {
    unchar lenSt[4];
    utoa(lenSt, len, 10);
    
    txBufAdd_("AT+CIPSEND=");
    txBufAddChar(linkID);
    txBufAddChar(',');
    txBufAddLn_(lenSt);
    txBufAddLn(data, len);
}

/**
 * Start the TCP server
 * 
 * @param port The port used by the server
 */
void wifi_startTCPServer(unchar *port) {
    txBufAddLn_("AT+CIPMUX=1"); // TCP Server requires for multiple connections to be enabled

    txBufAdd_("AT+CIPSERVER=");
    txBufAdd_("1,");
    txBufAddLn_(port);
}

/**
 * Setup Software AP
 * 
 * @param ssid  The SSID of the AP
 * @param pwd   The password of the AP
 */
void wifi_setSoftAP(unchar *ssid, unchar *pwd) {
    txBufAdd_("AT+CWSAP_CUR=");
    txBufAddStr_(ssid);
    txBufAdd_(",");
    txBufAddStr_(pwd);
    txBufAddLn_(",8,3");
}