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

unchar se_txData[256];
uint se_txLen = 0;

/**
 * Add a String
 * @param data  The string
 * @param len   The length of the string
 */
void se_addStr(unchar *data, uint len) {
    uint i;
    for (i = 0; i < len; i++) {
        se_txData[se_txLen] = data[i];
        se_txLen++;
    }
}

/**
 * Add a NULL terminated String
 * @param data  The string
 */
void se_addStr_(unchar *data) {
    int i;
    for (i = 0; data[i] != 0; i++) {
        se_txData[se_txLen] = data[i];
        se_txLen++;
    }
}

/**
 * Add a Character
 * @param ch The character to be added
 */
inline void se_addChar(unchar ch){
    se_txData[se_txLen] = ch;
    se_txLen++;
}

/**
 * Add an unsigned intiger
 * @param num   The intiger to be added
 */
void se_addUNum(uint num){
    if(num<10){
        se_addChar('0' + num);
        return;
    }
    unchar numStr[6];
    utoa(numStr, num, 10);
    se_addStr_(numStr);
}

/**
 * Add a signed intiger
 * @param num The Intiger
 */
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

/**
 * Add a high double byte intiger with a low double byte fraction
 * @param num   The number to be added
 * @param max   The Maximum number of decimals
 */
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

/**
 * Add a high byte intiger with a low triple byte fraction
 * @param num   The number to be added
 * @param max   The Maximum number of decimals
 */
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

/**
 * Clear the serializer
 */
inline void se_clear(){
    se_txLen=0;
}

/**
 * The the serializer data to a TCP client
 * @param id the ID of the client
 */
inline void se_sendToWifi(unchar id){
    wifi_send(se_txData, se_txLen, id);
}

/**
 * Add ERROR if nothing has been added
 */
inline void se_noEmpty(){
    if(se_txLen==0){
        se_addStr_("ERROR");
    }
}