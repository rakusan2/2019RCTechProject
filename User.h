/* 
 * File:   user.h
 * Author: Rakus
 *
 * Created on April 2, 2019, 11:04 AM
 */

#include "tools.h"

struct USER{
    unchar connected;
    unchar nl;
    unchar repeatCMD[20];
    uint repeatCMDLen;
    unchar repeatTime;
} users[5];

void user_connect(unchar id);
void user_disconnect(unchar id);
void user_doRepeat(uint time);