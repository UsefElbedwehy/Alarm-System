/*
 * Service_interface.h
 *
 *  Created on: May 18, 2024
 *      Author: Usef
 */

#ifndef SERVICE_INTERFACE_H_
#define SERVICE_INTERFACE_H_

#include "Service_prv.h"

void SRC_ServiceInit(void);
void SRV_SetTimeNDate(void);
void SRV_ShowTimeNDate(void);
void SRV_SetAlarm(uint8_t copy_u8AlarmNumber);

#endif /* SERVICE_INTERFACE_H_ */
