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

void __ISR(_SPI_1_VECTOR, IPL6SOFT)spiInt(){
    
}

void hb_init(){
    
    SDI1R = 0b0011; // Set SDI to Port B11
    RPA1R = 0b0011; // Set SDO to Port A1
    TRISAbits.TRISA0 = 0;   // Set SS pit on port A0 as output
    TRISBbits.TRISB10 = 0;  // Set H-Bridge Enable pin on Port B10 as output
    
    SPI1CON = 0x1000042D;
    SPI1CON2 = 0;
    SPI1BRG = 4;    // Set SCK Baud rate to 4.8 MHz
    
    IEC1bits.SPI1RXIE = 1;  // Enable SPI Receive Interrupt
    IPC7bits.SPI1IP = 6;    // Set interrupt priority to 6
    
    PORTBbits.RB10 = 1; // Enable H-Bridge
    SPI1CONSET=0x8000;  // Enable SPI
}

