#ifndef DS1307_INTERFACE_H_
#define DS1307_INTERFACE_H_

#include "DS1307_prv.h"

typedef enum{

	TIME_FORMATE_AM_PM_,
	TIME_FORMAT_24_

}TimeFormate_t;

typedef enum{

	AM_NOTATION_OR_24=0x00,
	PM_NOTATION

}AM_PM_Notation_t;

typedef enum{


	MONDAY=0x01,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
	SUNDAY

}Week_Day_t;

typedef enum
{
	BCD_FORMAT,
	DECIMAL_FORMAT


}RTC_Format_t;

typedef struct{

	TimeFormate_t				TIME_FORMAT;			/*AM/PM or 0x24*/
	AM_PM_Notation_t			AM_PM_NOTATION;			/*AM or PM*/
	uint8_t						SECONDS;			/*from 0x0 to 0x60*/
	uint8_t						MINUTES;			/*from 0x0 to 0x60*/
	uint8_t						HOURS;			/*from 0x0 to 0x24 or 0x12 */
	uint8_t						DAY;				/*From 0x1 to 0x30*/
	uint8_t						MONTH;			/*From 0x1 to 0x12*/
	uint8_t						YEAR;			/*2 bytes take 1st one byte*/
	Week_Day_t					WEEK_DAY;				/*from 0x1 to 0x7*/

}RTC_Config_t;

void RTC_SetTimeDate_(I2C_config_t* I2C_Config, RTC_Config_t* TimeAndDate ,RTC_Format_t Format);

void RTC_SetTimeDate(I2C_config_t* I2C_Config, RTC_Config_t* TimeAndDate ,RTC_Format_t Format);

void RTC_ReadTimeDate(I2C_config_t* I2C_Config, RTC_Config_t* TimeAndDate ,uint8_t* DT);

// Function to convert a decimal value to BCD
uint8_t DecimalToBCD(uint8_t decimal);
// Function to convert a BCD value to decimal
uint8_t BCDToDecimal(uint8_t bcd);


#endif
