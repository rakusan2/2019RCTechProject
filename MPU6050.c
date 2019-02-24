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
#include "MPU6050.h"
#include "serializer.h"
#include "deserializer.h"

#define MPU_ADDRESS         0b1101000
#define MPU_SMPLRT_DIV      0x19
#define MPU_CONFIG          0x1A
#define MPU_GYRO_CONFIG     0x1B
#define MPU_ACCEL_CONFIG    0x1C
#define MPU_FIFO_EN         0x23
#define MPU_USER_CTRL       0x6A
#define MPU_PWR_MGMT_1      0x6B
#define MPU_FIFO_COUNT_H    0x72
#define MPU_FIFO_R_W        0x74

#define rotate(a,b,c) a[b]=c;b=(b+1)&8

unchar interpretFIFO_ID;
unchar getFIFOLen_ID;

struct XYZ accel;
struct{
    int32_t X;
    int32_t Y;
    int32_t Z;
} gyro;
int16_t temp[8];
int tempIndex = 0;


void interpretFIFO(unchar *data, uint len){
    uint i;
    for(i=0;i<len;i+=14){
        rotate(temp, tempIndex, joinHL(data,i));
        rotate(accel.X, accel.xIndex, joinHL(data, i + 2));
        rotate(accel.Y, accel.yIndex, joinHL(data, i + 4));
        rotate(accel.Z, accel.zIndex, joinHL(data, i + 6));
        gyro.X += ((int32_t)joinHL(data, i + 8))*5;
        gyro.Y += ((int32_t)joinHL(data, i + 10))*5;
        gyro.Z += ((int32_t)joinHL(data, i + 12))*5;
    }
    mpu_data.accelX = average8(accel.X);
    mpu_data.accelY = average8(accel.Y);
    mpu_data.accelZ = average8(accel.Z);
}
void getFIFOLen(unchar *data, uint len){
    if(len == 2){
        i2c_getMany(MPU_ADDRESS, MPU_FIFO_R_W, joinHL(data, 0), interpretFIFO_ID);
    }
}

void mpu_refresh(){
    i2c_getMany(MPU_ADDRESS, MPU_FIFO_COUNT_H, 2, getFIFOLen_ID);
}

void mpu_serializeAccelData(){
    se_addStr_("AX=");
    se_addNum(mpu_data.accelX);
    se_addStr_(",AY=");
    se_addNum(mpu_data.accelY);
    se_addStr_(",AZ=");
    se_addNum(mpu_data.accelZ);
}
void mpu_serializeGyroData(){
    se_addStr_("GX=");
    se_add1616Num(gyro.X, 2);
    se_addStr_(",GY=");
    se_add1616Num(gyro.Y, 2);
    se_addStr_(",GZ=");
    se_add1616Num(gyro.Z, 2);
}
void mpu_accelDeserializer(unchar *data, uint len){
    if(len>0){
        unchar ch = data[0];
        if(ch == 'x'){
            se_addStr_("AX=");
            se_addNum(mpu_data.accelX);
        }else if(ch == 'y'){
            se_addStr_("AY=");
            se_addNum(mpu_data.accelY);
        }else if(ch == 'z'){
            se_addStr_("AZ=");
            se_addNum(mpu_data.accelZ);
        }else mpu_serializeAccelData();
    }else{
        mpu_serializeAccelData();
    }
}
void mpu_gyroDeserializer(unchar *data, uint len){
    if(len>0){
        unchar ch = data[0];
        if(ch == 'x'){
            se_addStr_("GX=");
            se_add1616Num(gyro.X, 2);
        }else if(ch == 'y'){
            se_addStr_("GY=");
            se_add1616Num(gyro.Y, 2);
        }else if(ch == 'z'){
            se_addStr_("GZ=");
            se_add1616Num(gyro.Z, 2);
        }else mpu_serializeAccelData();
    }else{
        mpu_serializeGyroData();
    }
}
void mpu_tempDeserializer(unchar *data, uint len){
    se_addStr_("T=");
    se_add1616Num((((int32_t)mpu_data.temp)*193) + 2394030,2);
}
void mpu_init(){
    
    i2c_setOne(MPU_ADDRESS, MPU_SMPLRT_DIV, 0x4);    // Set sampling Rate to 200Hz
    i2c_setOne(MPU_ADDRESS, MPU_CONFIG, 0x1);        // Set Low Pas Filter to about 200Hz
    i2c_setOne(MPU_ADDRESS, MPU_GYRO_CONFIG, 2<<3);  // Set Full scale Gyro Range to 1000deg/s
    i2c_setOne(MPU_ADDRESS, MPU_ACCEL_CONFIG, 2<<3); // Set Full scale Accel Range to 8g
    i2c_setOne(MPU_ADDRESS, MPU_FIFO_EN, 0xf8);      // Send Temp, Gyro XYZ, and Accel XYZ to FIFO
    i2c_setOne(MPU_ADDRESS, MPU_USER_CTRL, 0x40);    // Enable FIFO Register
    i2c_setOne(MPU_ADDRESS, MPU_PWR_MGMT_1, 1);      // Set clock source to Gyroscope X refference
    
    interpretFIFO_ID = i2c_onRecieve(interpretFIFO);
    getFIFOLen_ID = i2c_onRecieve(getFIFOLen);

    accel.xIndex = accel.yIndex = accel.zIndex = 0;
    gyro.X = gyro.Y = gyro.Z = 0;
    
}

