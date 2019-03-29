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
void steer_deserializer(unchar *data, uint len);
void steer_set(int speed);
void steer_init();
void steer_setPosLimit(int speed);
void steer_setNegLimit(int speed);
void steer_trigEnd();
void steer_trigCenter();