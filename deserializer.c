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


void (*defunc[26])(unchar *str, uint len)

inline uint isCapital(unchar ch){
    return ch >= 'A' && ch <= 'Z'
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
    for(index>= len){
        unchar ch = data[index] - 'A';
        index++;
        if(ch<26 && defunc[ch] != null){
            uint commandDataLen = findEnd(index, data, len);
            defunc[funcI](data+index, commandDataLen);
            index += commandDataLen;
            if(index<len){
                se_addChar(',');
            }
        }
    }
}

void dese_addDeserializer(unchar id, void *func){
    if(id >= 'A' && id <= 'Z'){
        defunc[id - 'A'] = func;
    }
}