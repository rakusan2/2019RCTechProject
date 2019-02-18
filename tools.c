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

typedef unsigned char unchar;
typedef unsigned int uint;
typedef unsigned long int uint16;

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

inline uint joinHL(unchar *data, uint start){
    return (data[start]<<8)& data[start+1];
}

uint average8(uint *data){
    uint16 average=0;
    uint i;
    for(i=0;i<8;i++){
        average+=data[i];
    }
    return average >> 3;
}
