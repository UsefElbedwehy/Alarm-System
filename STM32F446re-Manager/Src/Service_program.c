/*
 * Service_program.c
 *
 *  Created on: May 18, 2024
 *      Author: Usef
 */
#include <stdint.h>

#include "ErrType.h"
#include "Stm32F446xx.h"
#include "SYSTICK_interface.h"
#include "RCC_Interface.h"
#include "GPIO_interface.h"
#include "I2C_interface.h"
#include "USART_interface.h"
#include "DS1307_interface.h"
#include "Service_interface.h"

I2C_config_t 		I2C1_SysConfig;
extern USART_ConfigReg_t USART2_CFG;
RTC_Config_t RTC_DS1307_Config;

uint8_t Alarm[5][7]={0};

void SRC_ServiceInit(void)
{
	PinConfig_t GPIO_SDA =
	{
			.Port = PORTB, .PinNum = PIN7, .Mode  = ALTERNATIVE_FUNCTION, .OutputType = OPEN_DRAIN,
			.AltFunc  = AF4, .PullType = PULLUP, .Speed  = HIGH_SPEED
	};
	PinConfig_t GPIO_SCL=
	{
			.Port     = PORTB,	.PinNum        = PIN6,	.Mode          = ALTERNATIVE_FUNCTION,
			.OutputType = OPEN_DRAIN, .AltFunc  = AF4, .PullType = PULLUP, .Speed   = HIGH_SPEED
	};

	I2C1_SysConfig.I2C_NUMBER 				  = I2C_1					;
	I2C1_SysConfig.I2C_ACKNOWLEDGE 		      = I2C_ACK					;
	I2C1_SysConfig.I2C_SPEED_MODE 			  = SM_MODE					;
	I2C1_SysConfig.I2C_STRETCHING 			  = NO_STRETCH				;
	I2C1_SysConfig.I2C_SCL_FREQ 			  = 100						;
	I2C1_SysConfig.I2C_FREQ 				  = 16						;
	I2C1_SysConfig.I2C_PEC 				      = NO_PEC					;
	I2C1_SysConfig.I2C_ADD_MODE 			  = _7_BIT_SLAVE_ADD		;
	I2C1_SysConfig.I2C_OWN_ADD 			      = 0						;

	GPIO_u8PinInit(&GPIO_SDA);
	GPIO_u8PinInit(&GPIO_SCL);

	RCC_APB1EnableClock(I2C1EN);

}

void SRV_SetTimeNDate(void)
{

	uint8_t Local_u8TimeNDate[17];

	USART_TransmitString(&USART2_CFG, (uint8_t*)"\nSet Time and Date for the first time: ");
	USART_TransmitString(&USART2_CFG, (uint8_t*)"\n\033[1mHH:MM:SS DAY DD/MM/YY Format(0 for 12h, 1 for 24h)\r");
	//Hours
	ReadCurrentDnTFromUser(Local_u8TimeNDate);

	RTC_DS1307_Config.SECONDS   = StringToHex(Local_u8TimeNDate[5], Local_u8TimeNDate[4]);
	RTC_DS1307_Config.MINUTES   = StringToHex(Local_u8TimeNDate[3], Local_u8TimeNDate[2]);
	RTC_DS1307_Config.HOURS     = StringToHex(Local_u8TimeNDate[1], Local_u8TimeNDate[0]);
	RTC_DS1307_Config.WEEK_DAY  = DayOfWeekToNumber(&Local_u8TimeNDate[6]);
	RTC_DS1307_Config.DAY		= StringToHex(Local_u8TimeNDate[10], Local_u8TimeNDate[9]);
	RTC_DS1307_Config.MONTH  	= StringToHex(Local_u8TimeNDate[12], Local_u8TimeNDate[11]);
	RTC_DS1307_Config.YEAR  	= StringToHex(Local_u8TimeNDate[14], Local_u8TimeNDate[13]);

	RTC_DS1307_Config.TIME_FORMAT = Local_u8TimeNDate[15];

	RTC_DS1307_Config.AM_PM_NOTATION = Local_u8TimeNDate[16];

	RTC_SetTimeDate_(&I2C1_SysConfig,&RTC_DS1307_Config , 0);

}

void SRV_ShowTimeNDate(void)
{
	static uint8_t local_u8TimeNDate[RECEIVED_RTC_DATA_NUMBER]={0};
	static uint8_t Local_u8StringBridge[14]={0};
	uint8_t Local_u8Iterator;
	RTC_ReadTimeDate(&I2C1_SysConfig, &RTC_DS1307_Config, local_u8TimeNDate);

	local_u8TimeNDate[2] &=~ (0x60);

	for(Local_u8Iterator=0;Local_u8Iterator <= RECEIVED_RTC_DATA_NUMBER;Local_u8Iterator++)
	{
		//local_u8TimeNDate[Local_u8Iterator] = BCDToDecimal(local_u8TimeNDate[Local_u8Iterator]);
		HexToString(local_u8TimeNDate[Local_u8Iterator],&Local_u8StringBridge[Local_u8Iterator*2] );
	}
	USART_TransmitString(&USART2_CFG, (uint8_t*)"\nTime and Date:");
	USART_TransmitBuffer(&USART2_CFG, Local_u8StringBridge ,14);
}

void SRV_SetAlarm(uint8_t copy_u8AlarmNumber)
{
	uint8_t Local_u8Station[17];

	USART_TransmitString(&USART2_CFG, (uint8_t*)"\nSet Time and Date: ");
	USART_TransmitString(&USART2_CFG, (uint8_t*)"\n\033[1mHH:MM:SS DAY DD/MM/YY Format(0 for 12h, 1 for 24h)\r");

	ReadCurrentDnTFromUser(Local_u8Station);

	Alarm[copy_u8AlarmNumber][0]   = StringToHex(Local_u8Station[1], Local_u8Station[0]);
	Alarm[copy_u8AlarmNumber][1]   = StringToHex(Local_u8Station[3], Local_u8Station[2]);
	Alarm[copy_u8AlarmNumber][2]   = StringToHex(Local_u8Station[5], Local_u8Station[4]);
	Alarm[copy_u8AlarmNumber][3]   = DayOfWeekToNumber(&Local_u8Station[6]);
	Alarm[copy_u8AlarmNumber][4]   = StringToHex(Local_u8Station[10], Local_u8Station[9]);
	Alarm[copy_u8AlarmNumber][5]   = StringToHex(Local_u8Station[12], Local_u8Station[11]);
	Alarm[copy_u8AlarmNumber][6]   = StringToHex(Local_u8Station[14], Local_u8Station[13]);

	SYSTICK_DelayMs_IT(1000);
}

static uint8_t StringToHex(uint8_t Arg1,uint8_t Arg2)
{
	uint8_t Local_u8Hex;
	Local_u8Hex = ((Arg1 - 48) | ((Arg2 - 48) << 4));
	return Local_u8Hex;
}

static uint8_t DayOfWeekToNumber(uint8_t* copy_u8Day)
{
	uint8_t Local_u8DayNumber=0;

	if((copy_u8Day[1] == 'O') || (copy_u8Day[1]=='o')){
		//M'O'NDAY
		Local_u8DayNumber=MONDAY;
	}
	else if((copy_u8Day[1] == 'U') || (copy_u8Day[1]=='u')){

		if((copy_u8Day[0] == 'T') || (copy_u8Day[0]=='t')){
			//T'U'SDAY
			Local_u8DayNumber=TUESDAY;
		}else
		{//S'U'NDAY
			Local_u8DayNumber=SUNDAY;
		}
	}
	else if((copy_u8Day[1] == 'E') || (copy_u8Day[1]=='e')){
		//T'H'URSDAY
		Local_u8DayNumber=WEDNESDAY;
	}
	else if((copy_u8Day[1] == 'H') || (copy_u8Day[1]=='h')){
		//T'H'URSDAY
		Local_u8DayNumber=THURSDAY;
	}
	else if((copy_u8Day[1] == 'R') || (copy_u8Day[1]=='r')){
		//F'R'IDAY
		Local_u8DayNumber=FRIDAY;
	}
	else if((copy_u8Day[1] == 'A') || (copy_u8Day[1]=='a')){
		//S'A'TURDAY
		Local_u8DayNumber=SATURDAY;
	}

	return Local_u8DayNumber;
}

static void ReadCurrentDnTFromUser(uint8_t* copy_u8array)
{
	USART_ReceiveData(&USART2_CFG, &copy_u8array[0]);
	USART_ReceiveData(&USART2_CFG, &copy_u8array[1]);
	USART_TransmitData(&USART2_CFG, ':');
	//Min
	USART_ReceiveData(&USART2_CFG, &copy_u8array[2]);
	USART_ReceiveData(&USART2_CFG, &copy_u8array[3]);
	USART_TransmitData(&USART2_CFG, ':');
	//sec
	USART_ReceiveData(&USART2_CFG, &copy_u8array[4]);
	USART_ReceiveData(&USART2_CFG, &copy_u8array[5]);
	USART_TransmitData(&USART2_CFG, ' ');

	//Day
	USART_ReceiveData(&USART2_CFG, &copy_u8array[6]);
	USART_ReceiveData(&USART2_CFG, &copy_u8array[7]);
	USART_ReceiveData(&USART2_CFG, &copy_u8array[8]);
	USART_TransmitData(&USART2_CFG, ' ');

	//Date
	USART_ReceiveData(&USART2_CFG, &copy_u8array[9]);
	USART_ReceiveData(&USART2_CFG, &copy_u8array[10]);
	USART_TransmitData(&USART2_CFG, '/');
	//Month
	USART_ReceiveData(&USART2_CFG, &copy_u8array[11]);
	USART_ReceiveData(&USART2_CFG, &copy_u8array[12]);
	USART_TransmitData(&USART2_CFG, '/');
	//Year
	USART_ReceiveData(&USART2_CFG, &copy_u8array[13]);
	USART_ReceiveData(&USART2_CFG, &copy_u8array[14]);
	USART_TransmitData(&USART2_CFG, ' ');
	USART_ReceiveData(&USART2_CFG, &copy_u8array[15]);

	if(copy_u8array[15] == '0')
	{
		USART_TransmitString(&USART2_CFG, (uint8_t*)" AM or PM ?  ");
		USART_ReceiveData(&USART2_CFG, &copy_u8array[16]);
	}
}

/**********************/
void SysTick_Handler(void)
{
	uint8_t Local_u8Index1,Local_u8Index2;
	uint8_t Local_u8Validate=0;
	uint8_t Local_u8CurrentTD[7]={0};

	RTC_ReadTimeDate(&I2C1_SysConfig, &RTC_DS1307_Config, Local_u8CurrentTD);

	Local_u8CurrentTD[2] &=~ (0x60);
	for(Local_u8Index1=0 ; Local_u8Index1<5 ; Local_u8Index1++)
	{
		for(Local_u8Index2=0 ; Local_u8Index2<7 ; Local_u8Index2++)
		{
			if(Alarm[Local_u8Index1][Local_u8Index2] != Local_u8CurrentTD[Local_u8Index2])
			{
				Local_u8Validate=0;
				break;
			}
			else
			{
				Local_u8Validate++;
			}
			if(Local_u8Validate == 7)
			{
				USART_TransmitString(&USART2_CFG, (uint8_t*)"\r ALARM NOTIFICATION   ");
			}
		}
	}
	SYSTICK_DelayMs_IT(500);

}

static void HexToString(uint8_t copy_u8HexVal, uint8_t* copy_u8buffer)
{

	copy_u8buffer[0] = (copy_u8HexVal >> 4) + '0';
	copy_u8buffer[1] = (copy_u8HexVal & 0x0F) + '0';

}
