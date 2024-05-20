#ifndef		I2C_INTERFACE_H_
#define		I2C_INTERFACE_H_
#include "I2C_prv.h"
typedef enum{

	CR1_PE,
	CR1_SMBUS,
	CR1_SMBTYPE=3,
	CR1_ENARP,
	CR1_ENPEC,
	CR1_ENGC,
	CR1_NOSTRETCH,
	CR1_START,
	CR1_STOP,
	CR1_ACK,
	CR1_POS,
	CR1_PEC,
	CR1_ALERT,
	CR1_SWRST=15

}I2C_CR1;

typedef enum{

	CR2_FREQ_0,
	CR2_ITERREN=8,
	CR2_ITEVTN,
	CR2_ITBUFEN,
	CR2_DMAEN,
	CR2_LAST

}I2C_CR2;

typedef enum{

	SR1_SB			,
	SR1_ADDR		,
	SR1_BTF			,
	SR1_ADD10		,
	SR1_STOPF		,
	SR1_RxNE=6		,
	SR1_TxE     	,
	SR1_BERR        ,
	SR1_ARLO        ,
	SR1_AF          ,
	SR1_OVR         ,
	SR1_PECERR		,
	SR1_TIMEOUT=14	,
	SR1_SMBALERT

}I2C_SR1_t;

typedef enum{

	SR2_MSL,
	SR2_BUSY,
	SR2_TRA,
	SR2_GENCALL=4,
	SR2_SMBDEFAULT,
	SR2_SMBHOST,
	SR2_DUALF,
	SR2_PEC

}I2C_SR2_t;

typedef enum{

	OAR1_ADD0=0,
	OAR1_ADD1=1,
	OAR1_ADD8=8,
	OAR1_ADDMODE=15

}I2C_OAR1_t;

typedef enum{

	I2C_1,
	I2C_2,
	I2C_3

}I2C_Number_t;

typedef enum{

	I2C_NACK,
	I2C_ACK

}I2C_Ack_t;

typedef enum{

	WRITE_CMD,
	READ_CMD

}I2C_RW_t;

typedef enum{

	SM_MODE,
	FM_MODE

}I2C_SpeedMode_t;

typedef enum{

	STRETCH,
	NO_STRETCH

}I2C_Stretching_t;

typedef enum{

	NO_PEC,
	PEC_TRANSFER

}I2C_PacketError_t;

typedef enum{

	_7_BIT_SLAVE_ADD,
	_10_BIT_SLAVE_ADD

}I2C_AddMode_t;



typedef struct{

	I2C_Number_t 			I2C_NUMBER;				/*I2C1/I2C2/I2C3*/
	I2C_Ack_t 				I2C_ACKNOWLEDGE;			/*ACK/NACK*/
	I2C_SpeedMode_t			I2C_SPEED_MODE;			/*standard or fast mode*/
	I2C_Stretching_t		I2C_STRETCHING;
	I2C_PacketError_t		I2C_PEC;
	I2C_AddMode_t			I2C_ADD_MODE;
	uint16_t 				I2C_SCL_FREQ;
	uint8_t		 			I2C_FREQ;					/*From 2 MHZ to 50 MHZ*/
	uint8_t					I2C_OWN_ADD;

}I2C_config_t;
/*I2C Initialization*/



void MasterSendSlaveAddress(I2C_config_t* Init ,uint8_t Address);
void MasterSendData(I2C_config_t* Init, uint8_t u8Data );

ErrorState_t I2C_Init(I2C_config_t* Init);
ErrorState_t I2C_SendStartCondition(I2C_config_t* Init);
ErrorState_t I2C_SendStopCondition(I2C_config_t* Init);
ErrorState_t I2C_MasterTransmitData(I2C_config_t* Init ,uint8_t Address,uint8_t* u8Data ,uint8_t u8size);
ErrorState_t I2C_MasterReceiveData(I2C_config_t* Init ,uint8_t* u8Data );
ErrorState_t I2C_MasterRequestWrite(I2C_config_t* Init, uint8_t u8Address);
void I2C_SendDataPacket(I2C_config_t* Init ,uint8_t u8Data );
ErrorState_t I2C_MasterRequestRead(I2C_config_t* Init, uint8_t u8Address);
/********************************************************************************
 ****************** ### PRIVATE FUNCTION SECTION ### ***************************
 ********************************************************************************/


ErrorState_t I2C_EnableAck(I2C_config_t* Init);
ErrorState_t I2C_DisableAck(I2C_config_t* Init);
uint16_t ClearFlag(I2C_config_t* Init);
ErrorState_t WaitForBusyFlag(I2C_config_t* Init);
ErrorState_t WaitForR1Flag(I2C_config_t* Init ,I2C_SR1_t copy_FlagID);
ErrorState_t WaitForR2Flag(I2C_config_t* Init ,I2C_SR2_t copy_FlagID );
ErrorState_t ReadR1FlagReg(I2C_config_t* Init );





#endif
