
#include <stdint.h>
#include "Stm32F446xx.h"
#include "SYSTICK_prv.h"
#include "SYSTICK_interface.h"

/*
@fn 	SYSTICK_DelayMs
@brief	 Timer in ms
@param[in] Copy_u32TimeMs
@retval void
 */
void SYSTICK_DelayMs(uint32_t Copy_u32TimeMs)
{
	/*CLK SRC*/
	SYSTICK->SYSTICK_CSR |= (SET_BIT_MASK<<CLKSOURCE);
	
	/*Reset current SYSTICK counter value*/
	SYSTICK->SYSTICK_CVR = INITIAL_LOAD_VAL;
	
	/*RELOAD VALUE*/
	SYSTICK->SYSTICK_RVR = (Copy_u32TimeMs*(SYSTEM_FREQUANCY/_10POW3));
	
	/*Enable SYSTICK*/
	SYSTICK->SYSTICK_CSR |= (SET_BIT_MASK<<ENABLE);
	
	/*Busy waiting count flag*/
	while((((SYSTICK->SYSTICK_CSR)>>COUNTFLAG)&GET_NUM_MASK) != READY_FLAG)
	{
		/*do nothing*/
	}
}
/*
@fn 	SYSTICK_DelayUs
@brief	 Timer in Ms
@param[in] Copy_u32TimeUs
@retval void
 */
void SYSTICK_DelayUs(uint32_t Copy_u32TimeUs)
{
	/*CLK SRC*/
	SYSTICK->SYSTICK_CSR |= (SET_BIT_MASK<<CLKSOURCE);
	
	/*RELOAD VALUE*/
	SYSTICK->SYSTICK_RVR = (Copy_u32TimeUs*(SYSTEM_FREQUANCY/_10POW6));
	
	/*Reset current SYSTICK counter value*/
	SYSTICK->SYSTICK_CVR = INITIAL_LOAD_VAL;
	
	/*Enable SYSTICK*/
	SYSTICK->SYSTICK_CSR |= (SET_BIT_MASK<<ENABLE);
	
	/*Busy waiting count flag*/
	while((((SYSTICK->SYSTICK_CSR)>>COUNTFLAG)&GET_NUM_MASK) != READY_FLAG)
	{
		/*do nothing*/
	}
}

/*
@fn 	SYSTICK_DelayMs_IT
@brief	 Timer in ms
@param[in] Copy_u32TimeMs
@retval void
 */
void SYSTICK_DelayMs_IT(uint32_t Copy_u32TimeMs)
{

	/*CLK SRC -> AHB/8 */
	SYSTICK->SYSTICK_CSR &=~ (SET_BIT_MASK << CLKSOURCE);
	
	/*Set reload value*/
	SYSTICK->SYSTICK_RVR = (Copy_u32TimeMs*(SYSTEM_FREQUANCY/_10POW3));

	/*Reset current SYSTICK counter value*/
	SYSTICK->SYSTICK_CVR = INITIAL_LOAD_VAL;

	/* Enable Exception For Busy Waiting */
	( SYSTICK->SYSTICK_CSR ) |= ( SET_BIT_MASK << TICKINT )  ;

	/*Enable SYSTICK*/
	SYSTICK->SYSTICK_CSR |= (SET_BIT_MASK << ENABLE);
	
}

/*
@fn 	SYSTICK_DelayUs_IT
@brief	 Timer in Ms
@param[in] Copy_u32TimeUs
@retval void
 */
void SYSTICK_DelayUs_IT(uint32_t Copy_u32TimeUs)
{

	/*CLK SRC -> AHB processor clock */
	SYSTICK->SYSTICK_CSR &=~ (SET_BIT_MASK << CLKSOURCE);
	
	/*RELOAD VALUE*/
	SYSTICK->SYSTICK_RVR = (Copy_u32TimeUs*(SYSTEM_FREQUANCY/_10POW6));

	/*Reset current SYSTICK counter value*/
	SYSTICK->SYSTICK_CVR = INITIAL_LOAD_VAL;

	/* Enable Exception For Busy Waiting */
	( SYSTICK->SYSTICK_CSR ) |= ( SET_BIT_MASK << TICKINT )  ;

	/*Enable SYSTICK*/
	SYSTICK->SYSTICK_CSR |= (SET_BIT_MASK << ENABLE);
	
}
