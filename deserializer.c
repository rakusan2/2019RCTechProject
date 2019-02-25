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

#include "serializer.h"
#include "sonic.h"
#include "turnSensor.h"
#include "MPU6050.h"
#include "tools.h"
#include "battery.h"
#include "SPIHBridge.h"


inline uint isCapital(unchar ch){
    return ch >= 'A' && ch <= 'Z';
}
uint findEnd(uint index, unchar *data, uint len){
    uint count=0;
    for(; index < len;index++){
        unchar ch = data[index];
        if(isCapital(ch)){
            return count;
        }else if(ch == '\\'){
            count++;
            index++;
        }
        count++;
    }
    return count;
}

void dese_deserialize(unchar *data, uint len){
    uint index=0;
    while(index< len){
        unchar ch = data[index];
        index++;
        uint commandDataLen = findEnd(index, data, len);
        switch(ch){
            case 'B':
                bat_serialize();
                break;
            case 'U':   // Ultrasound
                sonic_serializeData(data + index, commandDataLen);
                break;
            case 'D':   //Drive
                hb_driveDeserializer(data + index, commandDataLen);
                break;
            case 'S':   //Steer
                hb_steerDeserializer(data + index, commandDataLen);
                break;
            case 'L':   // Limit Switches
                ts_deserialize(data + index, commandDataLen);
                break;
            case 'A':   // Accelerometer
                mpu_accelDeserializer(data + index, commandDataLen);
                break;
            case 'G':   // Gyroscope
                mpu_gyroDeserializer(data + index, commandDataLen);
                break;
            case 'T':   // Temperature
                mpu_tempDeserializer(data + index, commandDataLen);
                break;
            case 'V':   // Version
                se_addStr_("V=0.1");
                break;
            case 'E':
                se_addStr_("E=\"");
                se_addStr(data + index, commandDataLen);
                se_addChar('"');
                break;
            default:
                continue;
        }
        index+=commandDataLen;
        if(index<len){
            se_addChar(',');
        }
    }
}