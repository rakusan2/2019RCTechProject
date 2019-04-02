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
#include "L298Steer.h"
#include "User.h"

/**
 * Check whether a character is a capital
 */
inline uint isCapital(unchar ch){
    return ch >= 'A' && ch <= 'Z';
}

/**
 * Find the Length of a command within a string
 * @param index The start of the Length count
 * @param data The string to be checked
 * @param len The Length of the input string
 */
uint findEnd(uint index, unchar *data, uint len){
    uint count=0;
    unchar quote=0;
    for(; index < len;index++){
        unchar ch = data[index];
        if(ch == '"'){
            quote = !quote;
        }else if(isCapital(ch) && !quote){
            return count;
        }else if(ch == '\\'){   // Skip over an escaped character
            count++;
            index++;
        }
        count++;
    }
    return count;
}


/**
 * Seperates a string to individual commands and sends them to their individual deserializers
 * @param userID The ID of the user requesting the data
 * @param data The string containing commands
 * @param len the length of the input string
 */
void dese_deserialize(unchar userID, unchar *data, uint len){
    uint index=0;
    USER user = users[userID];
    while(index< len){
        unchar ch = data[index];
        index++;
        uint commandDataLen = findEnd(index, data, len);
        switch(ch){
            case 'B':   // Battery
                bat_serialize();
                break;
            case 'U':   // Ultrasound
                sonic_serializeData(data + index, commandDataLen);
                break;
            case 'D':   //Drive
                //hb_driveDeserializer(data + index, commandDataLen);
                se_addStr_("D=NotImplemented");
                break;
            case 'S':   //Steer
                steer_deserializer(data + index, commandDataLen);
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
            case 'E':   // Echo
                se_addStr_("E=\"");
                se_addStr(data + index, commandDataLen);
                se_addChar('"');
                break;
            case 'N':   // New Line
                user.nl = !user.nl;
                se_addStr_("N=");
                se_addUNum(user.nl);
                break;
            case 'R':   // Repeat
                user_deseRepeat(userID, data + index, commandDataLen);
                break;
            default:
                se_addChar(ch);
                se_addStr_("=NotImplemented");
                continue;
        }
        index+=commandDataLen;
        if(index<len){
            se_addChar(',');
        }
    }
}