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

void i2c_init();
unchar i2c_onRecieve(void *func);
inline void i2c_getOne(unchar addr ,unchar reg, unchar id);
void i2c_getMany(unchar addr ,unchar reg, uint count, unchar id);
void i2c_setOne(unchar addr ,unchar reg, unchar data);
void i2c_setMany(unchar addr ,unchar reg, unchar *data, uint count);