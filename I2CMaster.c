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
/* ************************************************************************** */

#include <xc.h>
#include <sys/attribs.h>
#include "tools.h"

#define I2C_START   0x101
#define I2C_ACK     0x102
#define I2C_NACK    0x103
#define I2C_RESTART 0x104
#define I2C_END     0x105
#define I2C_READ    0x200

void (*receivers[10])();
uint receiverLen = 0;

uint i2c_txBuf[200];
uint txBufPointer = 0;
uint txEndPointer = 0;

unchar i2c_rxBuf[256];
uint _rxPointer=0;
uint rxID=0;

/**
 * Add a byte the the send buffer
 * @param byte The byte being added
 */
void bufAddByte(uint byte) {
    if(txEndPointer<200){                   // Do not Add if the buffer is filled
        i2c_txBuf[txEndPointer] = byte;
        txEndPointer++;
    }
}

void i2c_resend(){
    int i;
    for(i=txBufPointer-1;i>0;i--){
        uint data = i2c_txBuf[i];
        if(i2c_txBuf[i] == I2C_START){
            if(i2c_txBuf[i+1] & 1){
                data--;
                continue;
            }else{
                break;
            }
        }
    }
    I2C1STATbits.BCL = 0;
    __send();
}

/**
 * Send the send buffers contents
 */
void __send() {
    if(I2C1STATbits.BCL){
        i2c_resend();
    }
    if (!I2C1STATbits.TRSTAT && (txBufPointer < txEndPointer) && !I2C1CONbits.RCEN && !I2C1STATbits.ACKSTAT) {
        uint data = i2c_txBuf[txBufPointer];
        txBufPointer++;
        if (data >= I2C_READ) {
            rxID = data & 0xff;
            I2C1CONbits.RCEN = 1;
            return;
        }
        if (data > 0xff) {  // If it is a command
            switch (data) {
                case I2C_START:
                    I2C1CONbits.SEN = 1;
                    break;
                case I2C_RESTART:
                    I2C1CONbits.RSEN = 1;
                    break;
                case I2C_END:
                    I2C1CONbits.PEN = 1;
                    break;
                case I2C_ACK:
                    I2C1CONbits.ACKDT = 0;  // Select ACK
                    I2C1CONbits.ACKEN = 1;  // Send ACK
                    break;
                case I2C_NACK:
                    I2C1CONbits.ACKDT = 1;  // Select NACK
                    I2C1CONbits.ACKEN = 1;  // Send NACK
                    
                    // Check if received data then send it to the code that requested it
                    if(_rxPointer){
                        receivers[rxID](i2c_rxBuf,_rxPointer);
                        _rxPointer=0;
                    }
                    break;
            }
        } else {    // If it is not a command
            I2C1TRN = data;
        }
    }else if(txBufPointer == txEndPointer && txBufPointer > 0 && !I2C1STATbits.TRSTAT){
        //I2C1CONbits.PEN = 1;
        txBufPointer = txEndPointer = 0;
    }
}


/**
 * The interrupt to receive data
 */
void __ISR(_I2C_1_VECTOR, IPL5SOFT) I2CInt() {
    _nop();
    if(I2C1STATbits.BCL){
        i2c_resend();
    }else if (I2C1STATbits.RBF) {
        if(rxID < receiverLen){
            i2c_rxBuf[_rxPointer]=I2C1RCV;
            _rxPointer++;
        }else {
            I2C1RCV;    //Empty the receive buffer if there is nothing waiting for it
        }
        __send();
    } else if (!I2C1STATbits.ACKSTAT) {
        __send();
    }else if(!I2C1STATbits.TRSTAT && !I2C1CONbits.RCEN){
        __send();
    }
    IFS1bits.I2C1MIF=0;
}



/**
 * Start Writing to the recipient
 * @param addr  Address of the recipient
 * @param reg   The register to write to
 */
inline void startWrite(unchar addr, unchar reg) {
    bufAddByte(I2C_START);
    bufAddByte(addr<<1);
    bufAddByte(reg);
}

inline void _startReadReg(unchar addr, unchar reg){
    startWrite(addr, reg);
    bufAddByte(I2C_RESTART);
    bufAddByte((addr << 1) | 0x01);
}

/**
 * Set multiple sequential registers
 * @param addr  Address of the recipient
 * @param reg   The starting register to write to
 * @param data  Array of bytes to set
 * @param count The Length of the array
 */
void i2c_setMany(unchar addr ,unchar reg, unchar *data, uint count) {
    startWrite(addr, reg);
    uint i;
    for (i = 0; i < count; i++) {
        bufAddByte(data[i]);
    }
    bufAddByte(I2C_END);
    __send();
}

/**
 * Set a single register
 * @param addr  Address of the recipient
 * @param reg   The register to write to
 * @param data  Byte to set
 */
void i2c_setOne(unchar addr ,unchar reg, unchar data) {
    startWrite(addr, reg);
    bufAddByte(data);
    bufAddByte(I2C_END);
    __send();
}

/**
 * Get Data from multiple sequential registers
 * @param addr  Address of the recipient
 * @param reg   The starting register to read from
 * @param count The Length of the array
 * @param id    The ID of the interpreter function
 */
void i2c_getMany(unchar addr ,unchar reg, uint count, unchar id) {
    _startReadReg(addr, reg);
    uint i;
    bufAddByte(I2C_READ | id);
    for (i = 1; i < count; i++) {
        bufAddByte(I2C_ACK);
        bufAddByte(I2C_READ | id);
    }
    bufAddByte(I2C_NACK);
    bufAddByte(I2C_END);
    __send();
}

/**
 * Get Data from a single register
 * @param addr  Address of the recipient
 * @param reg   The register to read from
 * @param id    The ID of the interpreter function
 */
inline void i2c_getOne(unchar addr ,unchar reg, unchar id) {
    i2c_getMany(addr, reg, 1, id);
}

/**
 * Initialize the I2C
 */
void i2c_init() {
    //I2C1ADD = 0b1101000;
    I2C1BRG = 58; // Set Baud Rate to 400 kHz
    IPC8bits.I2C1IP = 5;
    IEC1bits.I2C1MIE = 1;
    I2C1CON = 0x8000;
    
}

/**
 * Register an interpreter
 * @param func  The interpreter
 * @return      The ID given to the interpreter
 */
unchar i2c_onRecieve(void *func){
    receivers[receiverLen] = func;
    uint r=receiverLen;
    receiverLen++;
    return r;
}