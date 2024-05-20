#include <stdint.h>

#include "Stm32F446xx.h"
#include "ErrType.h"

#include "I2C_prv.h"
#include "I2C_interface.h"

I2C_Reg_t* I2C[3] = {I2C1 ,I2C2 ,I2C3 };


ErrorState_t I2C_Init(I2C_config_t* Init)
{
	ErrorState_t Local_ErrState = I2C_OK;

	if(Init != NULL)
	{
		uint32_t CCR_VALUE;

		/*ENABLE PERIPHERAL*/
		I2C[Init->I2C_NUMBER]->I2C_CR1 &=~ (SET_BIT_MASK << CR1_PE);

		/*RESET I2C*/
		I2C[Init->I2C_NUMBER]->I2C_CR1 |= (1 << CR1_SWRST);
		I2C[Init->I2C_NUMBER]->I2C_CR1 &=~ (1 << CR1_SWRST);

		/*I2C APB peripheral clk*/
		I2C[Init->I2C_NUMBER]->I2C_CR2 &=~ (0x3F << CR2_FREQ_0);
		I2C[Init->I2C_NUMBER]->I2C_CR2 |= (Init->I2C_FREQ << CR2_FREQ_0);

		/*Rise Time*/
		I2C[Init->I2C_NUMBER]->I2C_TRISE &=~ 0x13;
		I2C[Init->I2C_NUMBER]->I2C_TRISE |= 0x11;

		/*calculate Rise Time*/
		/*STANDARD MODE*/
		I2C[Init->I2C_NUMBER]->I2C_CCR &=~ ( SET_BIT_MASK << 15 );
		I2C[Init->I2C_NUMBER]->I2C_CCR |= ( Init->I2C_SPEED_MODE << 15 );

		/*Calculate CCR Register Value*/
		CCR_VALUE = ((Init->I2C_FREQ) * (1000));
		CCR_VALUE /= (Init->I2C_SCL_FREQ);
		CCR_VALUE /= 2;
		/*Set Clock*/
		I2C[Init->I2C_NUMBER]->I2C_CCR |= CCR_VALUE ;

		/*clock Sretching*/
		I2C[Init->I2C_NUMBER]->I2C_CR1 &=~ (1 << CR1_NOSTRETCH);
		I2C[Init->I2C_NUMBER]->I2C_CR1 |= (Init->I2C_STRETCHING << CR1_NOSTRETCH);

		/*packet error cheching PEC*/
		I2C[Init->I2C_NUMBER]->I2C_CR1 &=~ (1 << CR1_PEC);
		I2C[Init->I2C_NUMBER]->I2C_CR1 |= (Init->I2C_PEC << CR1_PEC);

		/**/
		I2C[Init->I2C_NUMBER]->I2C_CR1 &=~ (1 << CR1_ENGC);

		/*Address mode*/
		I2C[Init->I2C_NUMBER]->I2C_OAR1 &=~ (1 << OAR1_ADDMODE);
		I2C[Init->I2C_NUMBER]->I2C_OAR1 |= (Init->I2C_ADD_MODE << OAR1_ADDMODE);

		/*Own address*/
		I2C[Init->I2C_NUMBER]->I2C_OAR1 &=~ (0x07F << OAR1_ADD1);
		I2C[Init->I2C_NUMBER]->I2C_OAR1 |= (Init->I2C_OWN_ADD << OAR1_ADD1);

		/*ENABLE PERIPHERAL*/
		I2C[Init->I2C_NUMBER]->I2C_CR1 |= (SET_BIT_MASK << CR1_PE);
	}
	else
	{
		Local_ErrState = I2C_NULL_POINTER;
	}

	return Local_ErrState;


}

/**/
ErrorState_t I2C_SendStartCondition(I2C_config_t* Init)
{

	ErrorState_t Local_ErrState = I2C_OK;

	if(Init != NULL)
	{
		/*START CONDITION*/
		I2C[Init->I2C_NUMBER]->I2C_CR1 |= (SET_BIT_MASK << CR1_START);
		/*wait for SB=1 , then write address to DR*/
		Local_ErrState = WaitForR1Flag(Init, SR1_SB);
	}
	else
	{
		Local_ErrState = I2C_NULL_POINTER;
	}

	return Local_ErrState;
}

/**/
ErrorState_t I2C_SendStopCondition(I2C_config_t* Init)
{

	ErrorState_t Local_ErrState = I2C_OK;

	if(Init != NULL)
	{
		I2C[Init->I2C_NUMBER]->I2C_CR1 |= (SET_BIT_MASK << CR1_STOP);
	}
	else
	{
		Local_ErrState = I2C_NULL_POINTER;
	}

	return Local_ErrState;
}
void MasterSendSlaveAddress(I2C_config_t* Init ,uint8_t Address)
{
	uint16_t Local_FlagVal;

	/*write address to DR*/
	I2C[Init->I2C_NUMBER]->I2C_DR = (Address << 1);
	/*wait ADDR=1*/
	WaitForR1Flag(Init, SR1_ADDR);
	/*Clear ADDR by reading SR1 and SR2*/
	Local_FlagVal = I2C[Init->I2C_NUMBER]->I2C_SR1;
	Local_FlagVal = I2C[Init->I2C_NUMBER]->I2C_SR2;
}

void MasterSendData(I2C_config_t* Init, uint8_t u8Data )
{
	//uint8_t u8Counter;


	WaitForR1Flag(Init, SR1_TxE);

	/*TXE=1, Write data*/
	I2C[Init->I2C_NUMBER]->I2C_DR = u8Data;

	WaitForR1Flag(Init, SR1_BTF);
}

ErrorState_t I2C_MasterTransmitData(I2C_config_t* Init ,uint8_t Address,uint8_t* u8Data ,uint8_t u8size)
{
	ErrorState_t Local_ErrState = I2C_OK;
	uint16_t Local_FlagVal=0;
	uint8_t u8Counter=0;

	if(Init != NULL)
	{

		WaitForBusyFlag(Init);

		//I2C_EnableAck(Init);

		/*START CONDITION*/
		I2C[Init->I2C_NUMBER]->I2C_CR1 |= (1 << CR1_START);
		/*wait for SB=1 , then write address to DR*/
		WaitForR1Flag(Init, SR1_SB);

		/*write address to DR*/
		I2C[Init->I2C_NUMBER]->I2C_DR = Address;
		/*wait ADDR=1*/
		WaitForR1Flag(Init, SR1_ADDR);
		/*Clear ADDR by reading SR1 and SR2*/
		Local_FlagVal = I2C[Init->I2C_NUMBER]->I2C_SR1;
		Local_FlagVal = I2C[Init->I2C_NUMBER]->I2C_SR2;

		for(u8Counter=0;u8Counter<u8size;u8Counter++)
		{
			WaitForR1Flag(Init, SR1_TxE);
			/*TXE=1, Write data*/
			I2C[Init->I2C_NUMBER]->I2C_DR = u8Data[u8Counter];

		}

		WaitForR1Flag(Init, SR1_BTF);
		//while( ((I2C[Init->I2C_NUMBER]->I2C_SR1 >> SR1_BTF) & GET_BIT_MASK) != I2C_READY_FLAG )
		/*Stop Condition*/
		I2C[Init->I2C_NUMBER]->I2C_CR1 |= (1 << CR1_STOP);

	}
	else
	{
		Local_ErrState = I2C_NULL_POINTER;
	}


	return Local_ErrState;
}

ErrorState_t I2C_MasterReceiveData(I2C_config_t* Init ,uint8_t* u8Data )
{
	ErrorState_t Local_ErrState = I2C_OK;

	if(Init != NULL)
	{
		WaitForR1Flag(Init, SR1_RxNE);

		*u8Data = (uint8_t)I2C[Init->I2C_NUMBER]->I2C_DR;
	}
	else
	{
		Local_ErrState = I2C_NULL_POINTER;
	}


	return Local_ErrState;
}










/*********************************************************************************************/
/*SendAddressPacketMTransmitter*/
ErrorState_t I2C_MasterRequestWrite(I2C_config_t* Init, uint8_t u8Address)
{
	ErrorState_t Local_ErrState = I2C_OK;

	if(Init != NULL)
	{
		Local_ErrState = WaitForBusyFlag(Init);
		if(Local_ErrState ==  I2C_TIMEOUT)
		{
			I2C[Init->I2C_NUMBER]->I2C_CR1 |= (1 << CR1_SWRST);
			I2C[Init->I2C_NUMBER]->I2C_CR1 &=~ (1 << CR1_SWRST);

			Local_ErrState = I2C_Init(Init);
		}

		//I2C_EnableAck(Init);

		/*START CONDITION*/
		I2C_SendStartCondition(Init);

		/*write address to DR*/
		I2C[Init->I2C_NUMBER]->I2C_DR = (uint32_t)u8Address;

		/*wait ADDR=1*/
		WaitForR1Flag(Init, SR1_ADDR);

		/*Clear ADDR by reading SR1 and SR2*/
		ClearFlag(Init);

	}
	else
	{
		Local_ErrState = I2C_NULL_POINTER;
	}

	return Local_ErrState;
}

void I2C_SendDataPacket(I2C_config_t* Init ,uint8_t u8Data )
{
	WaitForR1Flag(Init, SR1_TxE);
	/*TXE=1, Write data*/
	I2C[Init->I2C_NUMBER]->I2C_DR = u8Data;

	WaitForR1Flag(Init, SR1_BTF);
}


/*I2C_SendAddressPacketMReceiver*/
ErrorState_t I2C_MasterRequestRead(I2C_config_t* Init, uint8_t u8Address)
{
	ErrorState_t Local_ErrState = I2C_OK;

	if(Init != NULL)
	{
		I2C_EnableAck(Init);

		/*START CONDITION*/
		I2C_SendStartCondition(Init);

		/*write address to DR*/
		I2C[Init->I2C_NUMBER]->I2C_DR = u8Address;



		/*wait ADDR=1*/
		Local_ErrState = WaitForR1Flag(Init, SR1_ADDR);

		/*Clear ADDR by reading SR1 and SR2*/
		ClearFlag(Init);

	}
	else
	{
		Local_ErrState = I2C_NULL_POINTER;
	}

	return Local_ErrState;
}


/********************************************************************************/

/********************************************************************************
 ****************** ### PRIVATE FUNCTION SECTION ### ***************************
 ********************************************************************************/

ErrorState_t I2C_EnableAck(I2C_config_t* Init)
{

	ErrorState_t Local_ErrState = I2C_OK;

	if(Init != NULL)
	{
		I2C[Init->I2C_NUMBER]->I2C_CR1 |= (SET_BIT_MASK << 10);
	}
	else
	{
		Local_ErrState = I2C_NULL_POINTER;
	}

	return Local_ErrState;
}

ErrorState_t I2C_DisableAck(I2C_config_t* Init)
{

	ErrorState_t Local_ErrState = I2C_OK;

	if(Init != NULL)
	{
		I2C[Init->I2C_NUMBER]->I2C_CR1 &=~ (SET_BIT_MASK << 10);
	}
	else
	{
		Local_ErrState = I2C_NULL_POINTER;
	}

	return Local_ErrState;
}

uint16_t ClearFlag(I2C_config_t* Init)
{
	uint16_t Local_FlagVal = 0;

	Local_FlagVal = I2C[Init->I2C_NUMBER]->I2C_SR1;
	Local_FlagVal = I2C[Init->I2C_NUMBER]->I2C_SR2;

	return Local_FlagVal;
}

ErrorState_t WaitForBusyFlag(I2C_config_t* Init)
{
	ErrorState_t Local_ErrState = I2C_OK;

	uint32_t Loacl_u32TimeOut=0;
	while( ((I2C[Init->I2C_NUMBER]->I2C_SR2 >> SR2_BUSY) & GET_BIT_MASK) == 1 )
	{
		Loacl_u32TimeOut++;
		if(Loacl_u32TimeOut > IIC_MAX_TIMEOUT)
		{
			return I2C_TIMEOUT;
		}
	}
	return Local_ErrState;
}

ErrorState_t WaitForR1Flag(I2C_config_t* Init ,I2C_SR1_t copy_FlagID)
{
	ErrorState_t Local_ErrState = I2C_OK;

	uint32_t Loacl_u32TimeOut=0;
	while( ((I2C[Init->I2C_NUMBER]->I2C_SR1 >> copy_FlagID)&GET_BIT_MASK) != I2C_READY_FLAG )
	{
		Loacl_u32TimeOut++;
		if(Loacl_u32TimeOut > IIC_MAX_TIMEOUT)
		{
			return I2C_TIMEOUT;
		}
	}
	return Local_ErrState;
}

ErrorState_t WaitForR2Flag(I2C_config_t* Init ,I2C_SR2_t copy_FlagID )
{
	ErrorState_t Local_ErrState = I2C_OK;

	uint32_t Loacl_u32TimeOut=0;
	while( ((I2C[Init->I2C_NUMBER]->I2C_SR2 >> copy_FlagID)&GET_BIT_MASK) != I2C_READY_FLAG )
	{
		Loacl_u32TimeOut++;
		if(Loacl_u32TimeOut > IIC_MAX_TIMEOUT)
		{
			return I2C_TIMEOUT;
		}
	}
	return Local_ErrState;
}
ErrorState_t ReadR1FlagReg(I2C_config_t* Init )
{
	uint16_t Local_FlagVal = 0;

	if(Init == NULL)
	{
		Local_FlagVal = I2C_NULL_POINTER;
	}
	else
	{

		Local_FlagVal = I2C[Init->I2C_NUMBER]->I2C_SR1;

	}

	return Local_FlagVal;
}
/********************************************************************************/
















