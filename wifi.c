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

#define UART1_PRIORITY 7
#define TXBufSize 2000
#define RXBufSize 2000

unchar txBuf[TXBufSize];
unchar rxBuf[RXBufSize];
uint txBufLen = 0;
uint rxBufLen = 0;
uint txPointer = 0;
uint rxPointer = 0;

int pauseTX = 0;
// Located In Main
void wifi_receive(unsigned char *data, unsigned int len);

/**
 * Try to add a byte to the TX buffer
 */
void __sendNext() {
    if (U1STAbits.UTXBF) {
        IEC1bits.U1TXIE = 1; // If the TX Buffer is full then set an interrupt for when it is not
    } else if ((txPointer < txBufLen) && !pauseTX) {
        //if (!txPointer) {
        //    PORTBbits.RB5 = 1;
        //}
        U1TXREG = txBuf[txPointer]; // Add next byte to the buffer
        if (txBuf[txPointer] == '\n') {
            pauseTX = 1; // Pause transmission when a new line is sent
        }
        txPointer++;
        if (txPointer == txBufLen) {
            txPointer = txBufLen = 0; // Restart filling the buffer when the end is reached
            //PORTBbits.RB5 = 0;
        }
        __sendNext();
    } else {
        IEC1bits.U1TXIE = 0; // Disable the TX interrupt when the buffer is not full
    }
}

/**
 * Test received data for whether transmission should resume or reset
 * 
 * @param data  Received data
 * @param len   Length of received data
 */
void resumeTX(unchar *data, uint len) {
    if (startsWith(data, len, "FAIL", 4)) {
        txBufLen = txPointer = 0;
        wifi_receive(data, len);
    } else if(len>0 && !startsWith(data,len,"AT",2) && !startsWith(data,len,"busy",4)) {
        pauseTX = 0;
        wifi_receive(data, len);
        __sendNext();
    }
    //if(startsWith(data,len,"SEND",4) || startsWith(data,len,"OK",2)){
    //    PORTBbits.RB5=1;
    //}
}

/**
 * Add a character to the transmission buffer
 * 
 * @param c character to be added
 */
inline void txBufAddChar(unchar c) {
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
void txBufAdd_(unchar *data) {
    uint i = 0;
    while (data[i] != '\0') {
        txBuf[txBufLen + i] = data[i];
        i++;
    }
    txBufLen += i;
}

/**
 * Add an Array of characters to the TX buffer surrounded by quotations
 * 
 * @param data  Array of characters
 * @param len   Length of the array
 */
inline void txBufAddStr(unchar *data, uint len) {
    txBufAddChar('"');
    txBufAdd(data, len);
    txBufAddChar('"');
}

/**
 * Add a null terminated Array of characters to the TX buffer surrounded by quotations
 * 
 * @param data  Array of characters
 */
inline void txBufAddStr_(unchar *data) {
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
    //PORTBbits.RB5 = 1;
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
    //PORTBbits.RB5 = 1;
}

/**
 * Get Number of data bytes in IPD response
 * 
 * @param data  Array of characters
 * @param collonLocation Location of ':' in IPD response
 */
int getIPDLen(unchar *data, int collonLocation){
    int i = collonLocation - 1;
    int num = 0;
    int multiplier = 1;
    while(data[i]!=',' && i > 0){
        num += (data[i]-'0') * multiplier;
        multiplier*=10;
        i--;
    }
    if(i<4){
        return -1;
    }
    return num;
}

signed int toRead = -1;

/**
 * UART Interrupt handler
 */
void __ISR(_UART_1_VECTOR, IPL7SOFT) UARTInt() {
    if (IFS1bits.U1RXIF) {
        unchar tempChar;
        while (U1STAbits.URXDA) { // While there are data in the receive buffer
            rxBuf[rxPointer] = tempChar = U1RXREG;
            
            if (pauseTX && rxPointer == 0 && tempChar == '>') { // Resume when READY to transmit TCP DATA
                pauseTX = 0;
                __sendNext();
            }

            // On an IPD response get the number of characters to read
            if(tempChar == ':' && startsWith(rxBuf, rxPointer, "+IPD", 4)){
                toRead = getIPDLen(rxBuf, rxPointer);
            }
            if(toRead == 0){ // Stop buffering received data when there is nothing to read
                tempChar='\n';
                rxPointer+=2;
                toRead=-1;
            }else if(toRead>0){
                toRead--;
            }

            // Non IPD responses are new line terminated
            if (tempChar == '\n' && toRead < 0) {
                resumeTX(rxBuf, rxPointer-1);
                rxPointer = 0;
                __sendNext();
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

    //TRISBbits.TRISB5 = 0;   // Set LED pin to output
    TRISAbits.TRISA4 = 0;   // Set WiFi RESET pin to output
    TRISBbits.TRISB13 = 1;  // Set RX pin to input
    TRISBbits.TRISB15 = 0;  // Set TX pin to input
    
    //PORTBbits.RB5 = 1;      // Light the LED
    PORTAbits.RA4 = 1;      // Set RESET pit to HIGH to enable the Wifi module
    //ANSELBbits.ANSB13 = 0;

    //TRISBCLR = 0b11 << 5; // Set Ports for LED and WiFi RESET as outputs
    //PORTBSET = 0b11 << 5; // Set both ports High
    //int i;
    //for (i = 0; i < TXBufSize; i++) {
    //    txBuf[i] = 0;
    //}
    //for (i = 0; i < RXBufSize; i++) {
    //    rxBuf[i] = 0;
    //}

    U1RXR = 0x03; // Set UART1 RX to Port B13
    RPB15R = 0x01; // Set UART1 TX to Port B15
    
    //CFGCONbits.IOLOCK = 0;
    U1STA = 0x1400;
    U1BRG = 0x0019;
    U1MODE = 0x8000;
    //CFGCONbits.IOLOCK = 1;

    IPC8bits.U1IP = UART1_PRIORITY; // Set Priority Level
    IEC1bits.U1RXIE = 1; // Enable Receive Interrupt
    // Transmit Interrupt needs to be enabled and disabled on the fly

    //wait(100000);
//    volatile uint j,i;
//    for (j=0;j<10;j++)
//    {
//        for (i=0;i<655350;i++);
//        PORTBbits.RB5 ^= 1;
//    }   
    //PORTBbits.RB5 = 0;
    txBufAddLn_("ATE0");
    //txBufAddLn_("AT+RESTORE");
}

/**
 * Send TCP data
 * 
 * @param data      Array of characters to be sent
 * @param len       Length of the Array
 * @param linkID    Recipient ID
 */
void wifi_send(unchar *data, uint len, unchar linkID) {
    // AT+CIPSEND=<linkID>,<Lenght>
    
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
    
    // AT+CIPSERVER=<Create/Destroy>,<IP Port>
    txBufAdd_("AT+CIPSERVER=1,");
    txBufAddLn_(port);
}

/**
 * Setup Software AP
 * 
 * @param ssid  The SSID of the AP
 * @param pwd   The password of the AP
 */
void wifi_setSoftAP(unchar *ssid, unchar *pwd) {
    // AT+CWSAP="<SSID>","<Password>",<Channel>,<Type>
    txBufAdd_("AT+CWSAP_CUR=");
    txBufAddStr_(ssid);
    txBufAdd_(",");
    txBufAddStr_(pwd);
    txBufAddLn_(",1,3");
}