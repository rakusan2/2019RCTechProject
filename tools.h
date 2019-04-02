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

typedef unsigned char unchar;
typedef unsigned int uint;

int startsWith(unchar *str, uint strLen, unchar *start, uint startLen);
inline uint joinHL(unchar *data, uint start);
int16_t average8(int16_t *data);
void cycleDelay(uint cycles);

#define isBetween(a,b,c) (a <= b && b <= c)
#define bind(a,b,c) max(min(b,c),a)