#include <stdint.h>
#include  "ErrType.h"

#include "I2C_prv.h"
#include "I2C_interface.h"

#include "DS1307_prv.h"
#include "DS1307_interface.h"

void RTC_SetTimeDate_(I2C_config_t* I2C_Config, RTC_Config_t* TimeAndDate ,RTC_Format_t Format)
{
	static uint8_t Local_u8DateTime[8] = {0};
	uint8_t Local_u8Iterator;
	Local_u8DateTime[0] = 0x00;


	/*Seconds*/
	Local_u8DateTime[1] = (TimeAndDate->SECONDS);
	/* it is important to enable the oscillator (CH bit=0) during initial configuration.*/
	Local_u8DateTime[1] &=~ (1 << 7);
	/*Minutes*/
	Local_u8DateTime[2] = (TimeAndDate->MINUTES);

	/*24 time format*/
	if(TimeAndDate->TIME_FORMAT == '1')
	{
		Local_u8DateTime[3] = (TimeAndDate->HOURS);
		Local_u8DateTime[3] &=~ (1 << 6);
	}
	else
	{
		if(TimeAndDate->AM_PM_NOTATION == '0')
		{
			Local_u8DateTime[3] = (TimeAndDate->HOURS);
		}
		else
		{
			Local_u8DateTime[3] = (TimeAndDate->HOURS);
			Local_u8DateTime[3] |= (1 << 5);
		}
		Local_u8DateTime[3] |= (1 << 6);

	}

	Local_u8DateTime[4] = (TimeAndDate->WEEK_DAY);

	Local_u8DateTime[5] = (TimeAndDate->DAY);

	Local_u8DateTime[6] = (TimeAndDate->MONTH);

	Local_u8DateTime[7] = (TimeAndDate->YEAR);

	if(Format == DECIMAL_FORMAT)
	{
		for(Local_u8Iterator=0 ; Local_u8Iterator<8 ;Local_u8Iterator++)
		{
			Local_u8DateTime[Local_u8Iterator] = DecimalToBCD(Local_u8DateTime[Local_u8Iterator]);
		}
	}

	I2C_MasterTransmitData(I2C_Config, WRITE_DS1307_RTC_ADDRESS, Local_u8DateTime, 7);
}

void RTC_SetTimeDate(I2C_config_t* I2C_Config, RTC_Config_t* TimeAndDate ,RTC_Format_t Format)
{
	static uint8_t Local_u8DateTime[8] = {0};

	Local_u8DateTime[0] = 0x00;

	if(Format == BCD_FORMAT)
	{


		/*Seconds*/
		Local_u8DateTime[1] = (TimeAndDate->SECONDS);
		/* it is important to enable the oscillator (CH bit=0) during initial configuration.*/
		Local_u8DateTime[1] &=~ (1 << 7);
		/*Minutes*/
		Local_u8DateTime[2] = (TimeAndDate->MINUTES);

		/*24 time format*/
		if(TimeAndDate->TIME_FORMAT == TIME_FORMAT_24_)
		{
			Local_u8DateTime[3] = (TimeAndDate->HOURS);
			Local_u8DateTime[3] &=~ (1 << 6);
		}
		else
		{
			if(TimeAndDate->AM_PM_NOTATION == AM_NOTATION_OR_24)
			{
				Local_u8DateTime[3] = (TimeAndDate->HOURS);
			}
			else
			{
				Local_u8DateTime[3] = (TimeAndDate->HOURS);
				Local_u8DateTime[3] |= (1 << 5);
			}
			Local_u8DateTime[3] |= (1 << 6);

		}

		Local_u8DateTime[4] = (TimeAndDate->WEEK_DAY);

		Local_u8DateTime[5] = (TimeAndDate->DAY);

		Local_u8DateTime[6] = (TimeAndDate->MONTH);

		Local_u8DateTime[7] = (TimeAndDate->YEAR);
	}
	else if(Format == DECIMAL_FORMAT)
	{
		Local_u8DateTime[1] &=~ (1 << 7);
		Local_u8DateTime[1] = DecimalToBCD(TimeAndDate->SECONDS);

		Local_u8DateTime[2] = DecimalToBCD(TimeAndDate->MINUTES);

		/*24 time format*/
		if(TimeAndDate->TIME_FORMAT == TIME_FORMAT_24_)
		{
			Local_u8DateTime[3] = DecimalToBCD(TimeAndDate->HOURS);
		}
		else
		{
			if(TimeAndDate->AM_PM_NOTATION == AM_NOTATION_OR_24)
			{
				Local_u8DateTime[3] = DecimalToBCD(TimeAndDate->HOURS);
			}
			else
			{
				Local_u8DateTime[3] = DecimalToBCD(TimeAndDate->HOURS);
				Local_u8DateTime[3] |= (1 << 5);
			}

		}

		Local_u8DateTime[4] = DecimalToBCD(TimeAndDate->WEEK_DAY);

		Local_u8DateTime[5] = DecimalToBCD(TimeAndDate->DAY);

		Local_u8DateTime[6] = DecimalToBCD(TimeAndDate->MONTH);

		Local_u8DateTime[7] = DecimalToBCD(TimeAndDate->YEAR);
	}

	I2C_MasterTransmitData(I2C_Config, WRITE_DS1307_RTC_ADDRESS, Local_u8DateTime, 8);
}


void RTC_ReadTimeDate(I2C_config_t* I2C_Config, RTC_Config_t* TimeAndDate ,uint8_t* DT)
{
	//static uint8_t Local_u8ReceivedBCDDateTime[7] = {0};

	uint8_t Local_u8Counter=0;

	I2C_MasterRequestWrite(I2C_Config, WRITE_DS1307_RTC_ADDRESS);

	I2C_SendDataPacket(I2C_Config, 0x00);

	I2C_MasterRequestRead(I2C_Config, READ_DS1307_RTC_ADDRESS);

	for(Local_u8Counter = 0 ; Local_u8Counter < 7 ; Local_u8Counter++ )
	{
		if(Local_u8Counter == 6)
		{
			I2C_DisableAck(I2C_Config);
		}
		I2C_MasterReceiveData(I2C_Config, &DT[Local_u8Counter]);
	}

	I2C_SendStopCondition(I2C_Config);

}

// Function to convert a decimal value to BCD
uint8_t DecimalToBCD(uint8_t decimal) {
	return (((decimal / 10) << 4) | (decimal % 10));
}

// Function to convert a BCD value to decimal
uint8_t BCDToDecimal(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}



