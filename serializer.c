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
#include "wifi.h"
#include <stdlib.h>
#include "tools.h"

unchar se_txData[1000];
uint se_txLen = 0;

void se_addStr(unchar *data, uint len) {
    uint i;
    for (i = 0; i < len; i++) {
        se_txData[se_txLen] = data[i];
        se_txLen++;
    }
}

void se_addStr_(unchar *data) {
    int i;
    for (i = 0; data[i] != 0; i++) {
        se_txData[se_txLen] = data[i];
        se_txLen++;
    }
}

inline void se_addChar(unchar ch){
    se_txData[se_txLen] = ch;
    se_txLen++;
}

void se_addUNum(uint num){
    if(num<10){
        se_addChar('0' + num);
        return;
    }
    unchar numStr[6];
    utoa(numStr, num, 10);
    se_addStr_(numStr);
}

void se_addNum(int num){
    if(num < 10 && num > -10){
        if(num < 0){
            se_addChar('-');
            num = -num;
        }
        se_addChar('0'+num);
    }else{
        unchar numStr[8];
        itoa(numStr, num, 10);
        se_addStr_(numStr);
    }
}

void se_add1616Num(int32_t num, uint max){
    se_addNum(num>>16);
    if(num < 0){
        num=-num;
    }
    num &= 0xffff;
    if(num == 0){
        return;
    }
    se_addChar('.');
    uint i;
    for(i=0;i<max;i++){
        num*=10;
        se_addChar('0'+(num >> 16));
        num &= 0xffff;
        if(num == 0){
            return;
        }
    }
}

void se_add0824Num(int32_t num, uint max){
    se_addNum(num>>24);
    if(num < 0){
        num=-num;
    }
    num &= 0xffffff;
    if(num == 0){
        return;
    }
    se_addChar('.');
    uint i;
    for(i=0;i<max;i++){
        num*=10;
        se_addChar('0'+(num >> 24));
        num &= 0xffffff;
        if(num == 0){
            return;
        }
    }
}

inline void se_clear(){
    se_txLen=0;
}

inline void se_sendToWifi(unchar id){
    wifi_send(se_txData, se_txLen, id);
}
inline void se_noEmpty(){
    if(se_txLen==0){
        se_addStr_("ERROR");
    }
}