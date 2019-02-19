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

#include "serializer.h"

uint ts_lastState;

void dese_deserialize(unchar *data, uint len);
void dese_addDeserializer(unchar id, void *func);
