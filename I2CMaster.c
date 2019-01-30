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
#define I2C_READ    0x200

void (*receivers[10])();
uint receiverLen = 0;

uint txBuf[200];
uint txBufPointer = 0;
uint txEndPointer = 0;
uint txPause = 0;

unchar rxBuf[1024];
uint _rxPointer=0;
uint rxID=0;

void bufAddByte(uint byte) {
    txBuf[txEndPointer] = byte;
    txEndPointer++;
}

void __send() {
    if (!txPause && !I2C1STATbits.TRSTAT && txBufPointer < txEndPointer) {
        uint data = txBuf[txBufPointer];
        if (data >= I2C_READ) {
            txPause = 1;
            I2C1CONbits.RCEN = 1;
            return;
        }
        txBufPointer++;
        if (data > 0xff) {
            switch (data) {
                case I2C_START:
                    I2C1CONSET = 1 << (txBufPointer > 0 ? 1 : 0);
                    break;
                case I2C_ACK:
                    I2C1CONbits.ACKDT = 0;
                    I2C1CONbits.ACKEN = 1;
                    break;
                case I2C_NACK:
                    I2C1CONbits.ACKDT = 1;
                    I2C1CONbits.ACKEN = 1;
                    break;
            }
        } else {
            I2C1TRN = data;
        }
    }else if(txBufPointer == txEndPointer && txBufPointer > 0 && !I2C1STATbits.TRSTAT){
        I2C1CONbits.PEN = 1;
        txBufPointer = txEndPointer = 0;
        if(_rxPointer){
            receivers[rxID](rxBuf,_rxPointer);
            _rxPointer=0;
        }
    }
}

void __ISR(_I2C_1_VECTOR, IPL5SOFT) I2CInt() {
    if (I2C1STATbits.RBF) {
        txPause = 0;
        uint id = txBuf[txBufPointer] & 0xff;
        if(id < receiverLen){
            rxID=id;
            rxBuf[_rxPointer]=I2C1RCV;
            _rxPointer++;
        }else {
            volatile int i = I2C1RCV;
        }
        txBufPointer++;
        __send();
    } else if (!I2C1STATbits.ACKSTAT) {
        __send();
    }
}

inline void startRead(unchar addr) {
    bufAddByte(I2C_START);
    bufAddByte(addr | 0x80);
}

inline void startWrite(unchar addr, unchar reg) {
    bufAddByte(I2C_START);
    bufAddByte(addr);
    bufAddByte(reg);
}

void i2c_setMany(unchar addr ,unchar reg, unchar *data, uint count) {
    startWrite(addr, reg);
    uint i;
    for (i = 0; i < count; i++) {
        bufAddByte(data[i]);
    }
    __send();
}

void i2c_setOne(unchar addr ,unchar reg, unchar data) {
    startWrite(addr, reg);
    bufAddByte(data);
    __send();
}

void i2c_getMany(unchar addr ,unchar reg, uint count, unchar id) {
    startWrite(addr, reg);
    startRead(addr);
    uint i;
    bufAddByte(I2C_READ | id);
    for (i = 1; i < count; i++) {
        bufAddByte(I2C_ACK);
        bufAddByte(I2C_READ | id);
    }
    bufAddByte(I2C_NACK);
    __send();
}

inline void i2c_getOne(unchar addr ,unchar reg, unchar id) {
    i2c_getMany(addr, reg, 1, id);
}

void i2c_init() {
    I2C1CON = 0x800;
    //I2C1ADD = 0b1101000;
    I2C1BRG = 58; // Set Baud Rate to 400 kHz
    IPC8bits.I2C1IP = 5;
    IEC1bits.I2C1MIE = 1;
    
}

unchar i2c_onRecieve(void *func){
    receivers[receiverLen] = func;
    receiverLen++;
    return receiverLen-1;
}