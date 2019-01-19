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

#define UART1_PRIORITY 6

// Located In Main
void wifi_recieve(unsigned char *data);

__ISR(_UART_1_VECTOR,IPL7SOFT) UARTInt(){
    unsigned char data[] = {5, 6,7,8};
    wifi_recieve(data);
}

void wifi_init(){
    
    U1RXR = 0x0011; // Set UART1 RX to Port B13
    RPB15R = 0b0001; // Set UART1 TX to Port B15

    U1MODE = 0b1000100010000000;
    U1STA = 0x1400;
    U1BRG = 0x0019;
    U1MODESET = 0x8000;
    
    IEC1 |= 1 << 8; // Enable Interrupt
    IPC8 |= (UART1_PRIORITY << 2);  // Set Priority Level
}

void wifi_send(unsigned char *data, int len){
    
}