/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

typedef unsigned char unchar;
typedef unsigned int uint;

int startsWith(unchar *str, uint strLen, unchar *start, uint startLen) {
    if (strLen < startLen)return 0;
    uint i;
    for (i = 0; i < startLen; i++) {
        if (str[i] != start[i]) {
            return 0;
        }
    }
    return 1;
}