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

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */

void wifi_init();
void wifi_send(unchar *data, uint len, unchar linkID);
void wifi_startTCPServer(unchar *port);
void wifi_setSoftAP(unchar *ssid, unchar *pwd);