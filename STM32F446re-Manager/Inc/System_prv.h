/*
 * SRV.h
 *
 *  Created on: May 14, 2024
 *      Author: Usef
 */

#ifndef SYSTEM_PRV_H_
#define SYSTEM_PRV_H_
/***LOGIN SYSTEM****/
#define MAX_ID_DIGITS 		4u
#define MAX_PASS_DIGITS 	MAX_ID_DIGITS
#define INITIAL_ID_VAL		0
#define INITIAL_PASS_VAL	0
#define INITIAL_INDEX		0
#define FALSE_AUTHENTICATED 		0
#define TRUE_AUTHENTICATED 		1
#define MAX_ATTEMPS				3

#define SET_TIME_N_DATE_OPTION		'1'
#define SHOW_TIME_N_DATE_OPTION		'2'
#define SET_ALARM_OPTION			'3'
#define EXIT_OPTION					'4'

#define FLASE_EXIT_REQ				0
#define TRUE_EXIT_REQ				1

/****************************************************************************************************/
static void ExitSystem(void);
/****************************************************************************************************/
#endif /* SYSTEM_PRV_H_ */
