/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#include "tools.h"
#include <xc.h>

void se_addStr(unchar *data, uint len);
void se_addStr_(unchar *data);
inline void se_addChar(unchar ch);
void se_addUNum(uint num);
void se_addNum(int num);
void se_add1616Num(int32_t num, uint max);
void se_add0824Num(int32_t num, uint max);
inline void se_clear();
inline void se_sendToWifi(unchar id);
inline void se_noEmpty();