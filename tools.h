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

typedef unsigned char unchar;
typedef unsigned int uint;

int startsWith(unchar *str, uint strLen, unchar *start, uint startLen);
inline uint joinHL(unchar *data, uint start);
uint average8(uint *data);