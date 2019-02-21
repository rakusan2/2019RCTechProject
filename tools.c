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

inline uint16_t joinHL(unchar *data, uint start){
    return (data[start]<<8) | data[start+1];
}

int16_t average8(int16_t *data){
    int32_t average=0;
    uint i;
    for(i=0;i<8;i++){
        average+=data[i];
    }
    return average >> 3;
}
