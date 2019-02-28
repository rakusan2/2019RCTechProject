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
#include "serializer.h"
#include "tools.h"

#define HB_MIN_TIME 0x7E

uint8_t hb_driveSpeed = 0;
uint8_t hb_onState = 0;
uint8_t hb_steerSpeed = 0;
uint16_t hb_timerCount = 0;
uint8_t hb_curCTRLState = 0;

/**
 * Set the Control register
 * @param data  The byte to set
 */
void hb_setCTRL(uint8_t data){
    if(hb_curCTRLState == data){
        return;
    }
    hb_curCTRLState = data;
    PORTAbits.RA0 = 0;
    cycleDelay(24);
    SPI1BUF = 0x83;
    SPI1BUF = data;
}

void __ISR(_SPI_1_VECTOR, IPL6SOFT)spiInt(){
    SPI1BUF;
    if(!SPI1STATbits.SPIBUSY){
        cycleDelay(24);
        PORTAbits.RA0 = 1;
    }
    
    IFS1bits.SPI1RXIF = 0;
}

void __ISR(_TIMER_3_VECTOR, IPL6SOFT)hbPWMInt(){
    uint8_t mask=0xff;
    if(hb_timerCount > hb_driveSpeed){
        mask &= 0xcc;
    }
    if(hb_timerCount > hb_steerSpeed){
        mask &= 0x33;
    }
    hb_setCTRL(hb_onState & mask);
    hb_timerCount++;
    
    if((hb_steerSpeed == 0 || hb_steerSpeed == 0x3f) && (hb_driveSpeed == 0 || hb_driveSpeed == 0x3f)){
        T1CONCLR = 0x8000;
        hb_timerCount = 0;
    }
    
    IFS0bits.T3IF = 0;
}

void hb_init(){
    
    SDI1R = 0b0011; // Set SDI to Port B11
    RPA1R = 0b0011; // Set SDO to Port A1
    TRISAbits.TRISA0 = 0;   // Set SS pit on port A0 as output
    TRISBbits.TRISB10 = 0;  // Set H-Bridge Enable pin on Port B10 as output
    
    PORTAbits.RA0 = 1;      // Set SS pin to deselect H-Bridge
    
    SPI1CON = 0x0000042D;
    SPI1CON2 = 0;
    //SPI1BRG = 4;    // Set SCK Baud rate to 4.8 MHz
    SPI1BRG = 20;
    
    IPC7bits.SPI1IP = 6;    // Set interrupt priority to 6
    IEC1bits.SPI1RXIE = 1;  // Enable SPI Receive Interrupt
    
    T3CON = 0x0060;         // 1:64 Prescaler
    PR3 = HB_MIN_TIME;
    
    IPC3bits.T3IP = 6;
    IPC3bits.INT3IS = 1;
    IEC0bits.T3IE = 1;
    
    PORTBbits.RB10 = 1; // Enable H-Bridge
    SPI1CONSET=0x8000;  // Enable SPI
}

void hb_setDrive(int16_t speed){
    if(speed<0){
        hb_driveSpeed = (-speed) & 0x3f;
        hb_onState = (hb_onState & 0b11001100) | 0b00010010;
    }else{
        hb_driveSpeed = speed & 0x3F;
        hb_onState = (hb_onState & 0b11001100) | 0b00100001;
    }
    if(!T3CONbits.ON){
        T3CONSET = 0x8000;
    }
}
void hb_setSteer(int16_t speed){
    if(speed<0){
        hb_steerSpeed = (-speed) & 0x3f;
        hb_onState = (hb_onState & 0b00110011) | 0b01001000;
    }else{
        hb_steerSpeed = speed & 0x3F;
        hb_onState = (hb_onState & 0b00110011) | 0b10000100;
    }
    if(!T3CONbits.ON){
        T3CONSET = 0x8000;
    }
}

signed int getMotorSet(unchar *data){
    unchar ch = data[0];
    if(ch == 'p'){
        return 0x3f;
    }else if(ch == 'n'){
        return -0x3f;
    }else if(ch == '\\'){
        return data[1];
    }else if(ch == '-' || (ch >= '0' && ch <= '9')){
        signed int num = 0;
        uint index = 0;
        signed int sign = 1;
        if(ch == '-'){
            index = 1;
            sign = -1;
        }
        ch = data[index];
        while((ch >= '0' && ch <= '9')){
            num = (num * 10) + ch - '0';
            index++;
            ch = data[index];
        }
        return num * sign;
    }else{
        return 0;
    }
}

void hb_driveDeserializer(unchar *data, uint len){
    if(len>0){
        hb_setDrive(getMotorSet(data));
    }
    se_addStr_("D=");
    if(hb_onState & 0x2){
        se_addChar('-');
    }
    se_addUNum(hb_driveSpeed);
}

void hb_steerDeserializer(unchar *data, uint len){
    if(len>0){
        hb_setSteer(getMotorSet(data));
    }
    se_addStr_("S=");
    if(hb_onState & 0x8){
        se_addChar('-');
    }
    se_addUNum(hb_steerSpeed);
}