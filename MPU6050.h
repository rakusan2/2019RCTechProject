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

#include <xc.h>
#include <sys/attribs.h>
#include "tools.h"


struct XYZ{
    uint X[8];
    uint Y[8];
    uint Z[8];
    uint xIndex;
    uint yIndex;
    uint zIndex;
    
};

struct MPUdata{
    uint accelX;
    uint accelY;
    uint accelZ;
    uint gyroX;
    uint gyroY;
    uint gyroZ;
} mpu_data;

void mpu_init();
void mpu_refresh();