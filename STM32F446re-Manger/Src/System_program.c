/*
 * SRV.c
 *
 *  Created on: May 14, 2024
 *      Author: Usef
 */
//System files are the main files that provide the application

#include <stdint.h>

#include "ErrType.h"
#include "Stm32F446xx.h"


#include "RCC_Interface.h"
#include "GPIO_interface.h"
#include "USART_interface.h"
#include "I2C_interface.h"
#include "System_interface.h"
#include "Service_interface.h"

USART_ConfigReg_t USART2_CFG;

extern I2C_config_t 		I2C1_SysConfig;

//uint8_t Global_u8Exit =FLASE_EXIT_REQ;
/****************************************************************************************************/

void SYS_SystemInit(void)
{

	PinConfig_t USART_TX_PIN = {
			.AltFunc=AF7, .Mode=ALTERNATIVE_FUNCTION, .OutputType=PUSH_PULL,
			.PinNum=PIN2, .Port=PORTA, .PullType=NOPULL, .Speed=LOW_SPEED
	};

	PinConfig_t USART_RX_PIN = {
			.AltFunc=AF7, .Mode=ALTERNATIVE_FUNCTION, .OutputType=PUSH_PULL,
			.PinNum=PIN3, .Port=PORTA, .PullType=NOPULL, .Speed=LOW_SPEED
	};



	RCC_AHB1EnableClock(GPIOAEN);
	RCC_AHB1EnableClock(GPIOBEN);


	GPIO_u8PinInit(&USART_TX_PIN);
	GPIO_u8PinInit(&USART_RX_PIN);

	SRC_ServiceInit();

	RCC_APB1EnableClock(USART2EN);

	USART2_CFG.USART_BAUDRATE=115200;
	USART2_CFG.USART_HWFLOWCONTROL=USART_HW_FLOW_CONTROL_OFF;
	USART2_CFG.USART_MODE=USART_RXTX;
	USART2_CFG.USART_OVERSAMPLINGMODE=USART_OVER16_;
	USART2_CFG.USART_PARITYBIT=USART_NO_PARITY;
	USART2_CFG.USART_STOPBITS=USART_ONE_STOP_BITS;
	USART2_CFG.USART_SYNCHMODE=USART_ASYNCH;
	USART2_CFG.USART_USARTNUMBER=USART_USART2;
	USART2_CFG.USART_WORDLENGTH=USART_EIGHT_BIT;



	I2C_Init(&I2C1_SysConfig);
	USART_Init(&USART2_CFG);

	USART_TransmitData(&USART2_CFG, '-');
}

void SYS_LoginSystem(void)
{
	uint8_t Local_u8UserID[MAX_ID_DIGITS] 		  = {INITIAL_ID_VAL};
	uint8_t Local_u8UserPassword[MAX_PASS_DIGITS] = {INITIAL_PASS_VAL};
	uint8_t Local_u8Iterator;
	uint8_t Local_u8ValidatePass=0;
	uint8_t Local_u8Attemps=0;
	uint8_t Local_u8Authenticated =FALSE_AUTHENTICATED;
	/*Read ID from user*/
	USART_TransmitString(&USART2_CFG, (uint8_t*)"Enter your ID: ");
	for(Local_u8Iterator=INITIAL_INDEX ; Local_u8Iterator<MAX_PASS_DIGITS ; Local_u8Iterator++)
	{
		USART_ReceiveData(&USART2_CFG, &Local_u8UserID[Local_u8Iterator]);
	}

	do
	{
		/*Read Password*/
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\nEnter your Password: ");
		Local_u8Iterator=0;
		for(Local_u8Iterator=INITIAL_INDEX ; Local_u8Iterator<MAX_PASS_DIGITS ; Local_u8Iterator++)
		{
			USART_ReceiveData(&USART2_CFG, &Local_u8UserPassword[Local_u8Iterator]);
		}

		/*Check Password*/
		for(Local_u8Iterator=INITIAL_INDEX ; Local_u8Iterator<MAX_PASS_DIGITS ; Local_u8Iterator++)
		{
			if(Local_u8UserPassword[Local_u8Iterator] == Local_u8UserID[MAX_ID_DIGITS - Local_u8Iterator -1])
			{
				Local_u8ValidatePass++;
			}
			else
			{
				Local_u8ValidatePass=0;
				break;
			}
		}
		if(Local_u8ValidatePass == MAX_PASS_DIGITS)
		{
			Local_u8Authenticated = TRUE_AUTHENTICATED;
		}
		else{
			Local_u8Attemps++;
			USART_TransmitString(&USART2_CFG, (uint8_t*)"\nWrong Password!");
		}

	}while((Local_u8Authenticated != TRUE_AUTHENTICATED) && (Local_u8Attemps != MAX_ATTEMPS));

	if(Local_u8Authenticated != TRUE_AUTHENTICATED)
	{
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\nMaximum login attempts reached. Access denied....Reset system amd try again.\n");
		while(1);
	}
	else
	{
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n---------------------------------------------");
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n(((((((( Welcome to the Alarm System )))))))))");
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n---------------------------------------------");
	}
}

void SYS_FirstSetUp(void)
{
	SRV_SetTimeNDate();

}

void SYS_DisplaySystem(void)
{

	uint8_t Local_u8Continue=0;
	static uint8_t Global_u8MenuCursor=0;

	uint8_t Local_u8StringContinue=0;

	while(Local_u8Continue != 1)
	{
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n     Choose option: ");
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n _______________________ ");
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n|                       |");
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n| 1- Set Time and Date. |");
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n| 2- Show Time and Date.|");
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n| 3- Set Alarm.         |");
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n| 4- Exit.              |");
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\n|_______________________|");

		USART_ReceiveData(&USART2_CFG, &Global_u8MenuCursor);

		switch(Global_u8MenuCursor)
		{
		case SET_TIME_N_DATE_OPTION:

			//Read Time and Date from user
			//Pass Time and date to DS1307 RTC
			SRV_SetTimeNDate();

			USART_TransmitString(&USART2_CFG, (uint8_t*)"\nTime and Date is set successfully. ;)");

			break;

		case SHOW_TIME_N_DATE_OPTION:

			//Read Time and Date from DS1307 RTC
			//Show Time and date to user
			SRV_ShowTimeNDate();
			//USART_TransmitString(&USART2_CFG, (uint8_t*)"\nDONE2");
			break;

		case SET_ALARM_OPTION:

			uint8_t Local_u8AlarmNumber=0;
			//Read Time and Date from user
			USART_TransmitString(&USART2_CFG, (uint8_t*)"\nWhich alarm you want to set?");
			USART_TransmitString(&USART2_CFG, (uint8_t*)"\nAlarm 1");
			USART_TransmitString(&USART2_CFG, (uint8_t*)"\nAlarm 2");
			USART_TransmitString(&USART2_CFG, (uint8_t*)"\nAlarm 3");
			USART_TransmitString(&USART2_CFG, (uint8_t*)"\nAlarm 4");
			USART_TransmitString(&USART2_CFG, (uint8_t*)"\nAlarm 5");

			USART_TransmitString(&USART2_CFG, (uint8_t*)"\nAlarm [");
			USART_ReceiveData(&USART2_CFG, &Local_u8AlarmNumber);
			USART_TransmitString(&USART2_CFG, (uint8_t*)"]");

			SRV_SetAlarm((Local_u8AlarmNumber-49));

			break;

		case EXIT_OPTION:	ExitSystem();		break;

		default: USART_TransmitString(&USART2_CFG, (uint8_t*)"\nWrong input. Try again."); break;
		}
		USART_TransmitString(&USART2_CFG, (uint8_t*)"\nContinue? [y/n] ");

		USART_ReceiveData(&USART2_CFG, &Local_u8StringContinue);
		if(Local_u8StringContinue == 'n')
		{
			Local_u8Continue = 1;
		}
		else
		{
			Local_u8Continue = 0;
		}

	}
	while(1);
}
/****************************************************************************************************/
/****************************************************************************************************/
static void ExitSystem(void)
{
	//Reset seq
	USART_TransmitString(&USART2_CFG, (uint8_t*)"\n\033[7mxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	USART_TransmitString(&USART2_CFG, (uint8_t*)"\n     System is out of service.");
	USART_TransmitString(&USART2_CFG, (uint8_t*)"\n     Reset to play again.");
	USART_TransmitString(&USART2_CFG, (uint8_t*)"\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

	while(1);
	//Global_u8Exit = TRUE_EXIT_REQ;

}
/****************************************************************************************************/
