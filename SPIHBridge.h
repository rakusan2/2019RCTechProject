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

void hb_init();
void hb_driveDeserializer(unchar *data, uint len);
void hb_steerDeserializer(unchar *data, uint len);
void hb_setDrive(int16_t speed);
void hb_setSteer(int16_t speed);