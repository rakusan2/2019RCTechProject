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


struct XYZ{
    int16_t X[8];
    int16_t Y[8];
    int16_t Z[8];
    int16_t xIndex;
    int16_t yIndex;
    int16_t zIndex;
};

struct MPUdata{
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
    int16_t temp;
} mpu_data;

void mpu_init();
void mpu_refresh();