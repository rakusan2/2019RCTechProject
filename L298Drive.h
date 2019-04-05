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

void drive_setCurSpeed(int speed);
void steer_set(int speed);
void drive_pwmRefresh();
signed int getMotorSet(unchar *data);
void drive_deserializer(unchar *data, uint len);
void drive_init();