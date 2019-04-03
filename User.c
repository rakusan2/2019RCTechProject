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
#include "tools.h"
#include "serializer.h"
#include "deserializer.h"
#include "User.h"

void user_connect(unchar id){
    users[id].connected = 1;
}

void user_disconnect(unchar id){
        users[id].connected = 0;
        users[id].nl = 0;
        users[id].repeatTime = 0;
        users[id].repeatCMDLen = 0;
}

void user_deseRepeat(unchar userID, unchar *data, uint len){
    unchar quote = 0;
    uint i;
    users[userID].repeatCMDLen = 0;
    users[userID].repeatTime = 0;
    for(i = 0; i < len; i++){
        unchar ch = data[i];
        if(ch == '"'){
            quote = !quote;
        }else if(!quote && isBetween('0', ch, '9')){
            users[userID].repeatTime = (users[userID].repeatTime * 10) + (ch - '0');
        }else if(quote){
            users[userID].repeatCMD[users[userID].repeatCMDLen] = ch;
            users[userID].repeatCMDLen++;
        }
    }
}

void user_doRepeat(uint time){
    uint i;
    for(i = 0; i < 5; i++){
        if(users[i].connected && users[i].repeatCMDLen > 0 && users[i].repeatTime > 0 && time % users[i].repeatTime == 0){
            dese_deserialize(i, users[i].repeatCMD, users[i].repeatCMDLen);
            se_noEmpty();
            se_sendToWifi(i);
            se_clear();
        }
    }
}