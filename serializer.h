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

#include "wifi.h"
#include "tools.h"

void se_addStr(unchar *data, uint len);
void se_addStr_(unchar *data);
inline void se_addChar(unchar ch);
void se_addUNum(uint num)
inline void se_clear();
inline se_sendToWifi(unchar id);