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
#include <stdlib.h>

typedef unsigned char unchar;
typedef unsigned int uint;

/**
 * 1st string starts with 2nd
 * @param str String to be checked
 * @param strLen Lenght of the string to be checked
 * @param start String to be checked with
 * @param startLen Length of the checking string
 */
int startsWith(unchar *str, uint strLen, unchar *start, uint startLen) {
    if (strLen < startLen)return 0;
    uint i;
    for (i = 0; i < startLen; i++) {
        if (str[i] != start[i]) {
            return 0;
        }
    }
    return 1;
}

/**
 * Join bytes to a 16 bit signed intiger
 * @param data Array to take the data from
 * @param start Index where to read from
 */
inline uint16_t joinHL(unchar *data, uint start){
    return (data[start]<<8) | data[start+1];
}

/**
 * Average 8 bytes in an array
 * @param data The Array of 8 integers
 */
int16_t average8(int16_t *data){
    int32_t average=0;
    uint i;
    for(i=0;i<8;i++){
        average+=data[i];
    }
    return average >> 3;
}

/**
 * Delay execution by certain number of cycles
 * @param number of cycles to delay by
 */
void cycleDelay(uint cycles){
    cycles>>=2;
    uint i;
    for(i=0;i<cycles;i++){
        _nop();
    }
}