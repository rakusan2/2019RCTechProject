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

#define MPU_ADDRESS     0b1101000
#define MPU_SMPLRT_DIV  0x19
#define MPU_CONFIG      0x1A
#define MPU_GYRO_CONFIG 0x1B
#define MPU_ACCEL_CONFIG 0x1C
#define MPU_FIFO_EN     0x23
#define MPU_USER_CTRL   0x6A
#define MPU_PWR_MGMT_1  0x6B

void interpretFIFO(unchar data){
}
unchar interpretFIFO_ID;
void mpu_init(){
    
    i2c_setOne(MPU_SMPLRT_DIV, 0x4);    // Set sampling Rate to 200Hz
    i2c_setOne(MPU_CONFIG, 0x1);        // Set Low Pas Filter to about 200Hz
    i2c_setOne(MPU_GYRO_CONFIG, 2<<3);  // Set Full scale Gyro Range to 1000deg/s
    i2c_setOne(MPU_ACCEL_CONFIG, 2<<3); // Set Full scale Accel Range to 8g
    i2c_setOne(MPU_FIFO_EN, 0xf8);      // Send Temp, Gyro XYZ, and Accel XYZ to FIFO
    i2c_setOne(MPU_USER_CTRL, 0x40);    // Enable FIFO Register
    i2c_setOne(MPU_PWR_MGMT_1, 1);      // Set clock source to Gyroscope X refference
    
    interpretFIFO_ID = i2c_onRecieve(interpretFIFO);
    
    // Enable 32-bit timer to get data from FIFO every 0.1s
    T4CON = T5CON = 0;
    T4CONSET = 0x8;
    TMR4 = 0;
    PR4 = 0x16e360;
    T4CONSET = 0x8000;
}