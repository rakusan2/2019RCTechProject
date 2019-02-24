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

inline uint isMotorSet(unchar c) {
    return (c >= '0' && c <= '9') || c == '+' || c == '-' || c == 'P' || c == 'N' || c == '\\';
}

signed int getMotorSet(uint *start, unchar *data) {
    uint index = *start;
    unchar c = data[index];
    if (c == 'P') {
        *start++;
        return 0xff;
    } else if (c == 'N') {
        *start++;
        return -0xff;
    } else if (c == '\\') {
        *start += 2;
        uint num = data[index + 1] << 1;
        return num > 0xff ? num & 0xff : num;
    } else {
        uint pos = 0x100;
        uint num = 0;
        if (c == '+') {
            index++;
            c = data[index];
        } else if (c == '-') {
            pos = 0;
            index++;
            c = data[index];
        }
        uint count = 0;
        while ((c >= '0' || c <= '9') && count < 3) {
            count++;
            num = (num * 10) + c - '0';
            index++;
            c = data[index];
        }
        *start = index;
        return pos ? num : -num;
    }
}

inline int isMotionSpecifier(unchar c) {
    return c == 'X' || c == 'Y' || c == 'Z';
}


void (*defunc[26])();

void dese_init(){
    uint i;
    for(i=0;i<26;i++){
        defunc[i] = NULL;
    }
}
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
                se_addStr_("Not Implemented");
                break;
            case 'S':   //Steer
                se_addStr_("Not Implemented");
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
            default:
                continue;
        }
        index+=commandDataLen;
        if(index<len){
            se_addChar(',');
        }
    }
}
//int count =0;
//int lastSet =0;
//void dese_addDeserializer(unchar id, void *func){
//    if(id >= 'A' && id <= 'Z'){
//        count++;
//        lastSet = id - 'A';
//        defunc[id - 'A'] = func;
//    }
//}