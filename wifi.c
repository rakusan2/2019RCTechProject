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

void resumeTX(unchar *data,uint len){
    if(startsWith(data,len,"FAIL",2)){
        txBufLen = txPointer = 0;
    }else{
        pauseTX=0;
    }
    wifi_receive(data,len);
}

inline void txBufAddChar(unchar c){
    txBuf[txBufLen] = c;
    txBufLen++;
}

void txBufAdd(unchar *data, uint len) {
    uint i;
    for (i = 0; i < len; i++) {
        txBuf[txBufLen + i] = data[i];
    }
    txBufLen += len;
}

void txBufAdd_(unchar *data){
    uint i=0;
    while(data[i] !='\0'){
        txBuf[txBufLen + i] = data[i];
        i++;
    }
    txBufLen+=i;
}

inline void txBufAddStr(unchar *data, uint len){
    txBufAddChar('"');
    txBufAdd(data,len);
    txBufAddChar('"');
}
inline void txBufAddStr_(unchar *data){
    txBufAddChar('"');
    txBufAdd_(data);
    txBufAddChar('"');
}

inline void txBufAddLn(unchar *data, uint len) {
    txBufAdd(data, len);
    txBufAdd("\r\n", 2);
}
inline void txBufAddLn_(unchar *data) {
    txBufAdd_(data);
    txBufAdd("\r\n", 2);
}

__ISR(_UART_1_VECTOR, IPL6SOFT) UARTInt() {
    if (IFS1bits.U1RXIF) {
        while (U1STAbits.URXDA) {
            rxBuf[rxPointer] = U1RXREG;
            if(pauseTX && rxBuf[rxPointer] == '>'){
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
    if (IFS1bits.U1TXIF) {
        __sendNext();
        IFS1bits.U1TXIF = 0;
    }
}

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

    U1MODE = 0b1000100010000000;
    U1STA = 0x1400;
    U1BRG = 0x0019;
    U1MODESET = 0x8000;

    IEC1bits.U1RXIE = 1; // Enable Receive Interrupt
    IEC1bits.U1TXIE = 1; // Enable Transmit Interrupt
    IPC8bits.U1IP = UART1_PRIORITY; // Set Priority Level
}

void wifi_send(unchar *data, uint len) {
    unchar lenSt[4];
    utoa(lenSt, len, 10);
    uint i;
    uint numLen = 0;
    for (i = 0; i < 4; i++) {
        if (lenSt[i] > '0') {
            numLen++;
        } else break;
    }
    txBufAdd_("AT+CIPSEND=");
    txBufAddLn(lenSt, numLen);
    txBufAddLn(data, len);
    __sendNext();
}

void wifi_startTCPServer(unchar *port) {
    txBufAddLn_("AT+CIPMUX=1");

    txBufAdd_("AT+CIPSERVER=");
    txBufAdd_("1,");
    txBufAddLn_(port);
    __sendNext();
}

void wifi_setSoftAP(unchar *ssid, unchar *pwd) {
    txBufAdd_("AT+CWSAP_CUR=");
    txBufAddStr_(ssid);
    txBufAdd_(",");
    txBufAddStr_(pwd);
    txBufAddLn_(",8,3");
    __sendNext();
}