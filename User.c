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
        struct USER user = users[id];
        user.connected = 0;
        user.nl = 0;
        user.repeatTime = 0;
        user.repeatCMDLen = 0;
}

void user_deseRepeat(unchar userID, unchar *data, uint len){
    struct USER user = users[userID];
    unchar quote = 0;
    uint i;
    for(i = 0; i < len; i++){
        unchar ch = data[i];
        if(ch == '"'){
            quote = !quote;
        }else if(!quote && isBetween('0', ch, '9')){
            user.repeatTime = (user.repeatTime * 10) + (ch - '0');
        }else if(quote){
            user.repeatCMD[user.repeatCMDLen] = ch;
            user.repeatCMDLen++;
        }
    }
}

void user_doRepeat(uint time){
    uint i;
    for(i = 0; i < 5; i++){
        struct USER user = users[i];
        if(user.connected && user.repeatCMDLen > 0 && user.repeatTime > 0 && time % user.repeatTime == 0){
            dese_deserialize(i, user.repeatCMD, user.repeatCMDLen);
        }
    }
}