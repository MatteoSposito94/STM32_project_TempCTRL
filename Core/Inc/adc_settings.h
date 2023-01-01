/*
 * adc_settings.h
 *
 *  Created on: Dec 31, 2020
 *      Author: andrea
 */

#ifndef INC_ADC_SETTINGS_H_
#define INC_ADC_SETTINGS_H_

/*

	TIMER generate SoC
			|
			v
	ADC generate Interrut at EoC


 */

#include "stm32f303x8.h"


 uint16_t ADC_BUFFER[2] = {};



void ADC_Setup(void){

	// Set the ADC with DMA

	// set PA5    and PA6
	//     ADC2/2     ADC2/3
	// ADC2 is on DMA1 CH2

	RCC -> AHBENR |= RCC_AHBENR_ADC12EN | RCC_AHBENR_GPIOAEN;
	RCC -> AHBENR |= RCC_AHBENR_DMA1EN;
	RCC -> CFGR2  |= RCC_CFGR2_ADCPRE12_DIV128;

	GPIOA -> MODER |= GPIO_MODER_MODER5_0 | GPIO_MODER_MODER5_1;
	GPIOA -> MODER |= GPIO_MODER_MODER6_0 | GPIO_MODER_MODER6_1;

//	ADC2 -> CFGR

	// Interrupt settings
	ADC2 -> IER |= ADC_IER_EOSIE;			// Enable end of sequence interrupt
	NVIC_EnableIRQ(ADC1_2_IRQn);

	ADC2 -> CFGR |= ADC_CFGR_DISCNUM_0; 	// 2 Channel to convert
	ADC2 -> CFGR |= ADC_CFGR_DISCEN;		// Enable discontinuous mode
	ADC2 -> CFGR &= ~ADC_CFGR_CONT;			// Disable continuous mode

	ADC2 -> CFGR |= ADC_CFGR_EXTEN_0;
	ADC2 -> CFGR &= ~ADC_CFGR_EXTEN_1;

	// Set the external trigger to TIM2 TRGO Event
	ADC2 -> CFGR |= ADC_CFGR_EXTSEL_3 | ADC_CFGR_EXTSEL_1 | ADC_CFGR_EXTSEL_0;

	// Set the external trigger for the SoC

	ADC2 -> CFGR &= ~(ADC_CFGR_RES_0 | ADC_CFGR_RES_1);		// 12 bit resolution

	ADC2 -> CFGR |= ADC_CFGR_DMACFG | ADC_CFGR_DMAEN;		// Enable DMA conversion and circular mode

	ADC2 -> SMPR1 |= ADC_SMPR1_SMP2_2 | ADC_SMPR1_SMP3_2;
	ADC2 -> SQR1  |= ADC_SQR1_L_0;
	ADC2 -> SQR1  |= (2 << 6) | (3 << 12);

	ADC1_2_COMMON -> CCR &= ~ (ADC12_CCR_CKMODE_1);
	ADC1_2_COMMON -> CCR |= ADC12_CCR_CKMODE_0;


	// DMA SETTINGS

	DMA1_Channel2 -> CCR |= DMA_CCR_PL_1 | DMA_CCR_PL_0;
	DMA1_Channel2 -> CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0;
	DMA1_Channel2 -> CCR &= ~(DMA_CCR_MSIZE_1 | DMA_CCR_PSIZE_1);
	DMA1_Channel2 -> CCR |= DMA_CCR_MINC;
	DMA1_Channel2 -> CCR &=  ~(DMA_CCR_PINC);
	DMA1_Channel2 -> CCR |= DMA_CCR_CIRC;
	DMA1_Channel2 -> CCR &= ~DMA_CCR_DIR;

	DMA1_Channel2 -> CNDTR = 2;

	DMA1_Channel2 -> CMAR = (uint32_t) (ADC_BUFFER);
	DMA1_Channel2 -> CPAR = (uint32_t) (&(ADC2 -> DR));

	DMA1_Channel2 -> CCR |= DMA_CCR_EN;





	// ADC CALIBRATION SETTINGS

	//ADC2 -> CR |= ADC_CR_ADVREGEN_0; 		// Enable voltage regulator
	//ADC2 -> CR &= ADC_CR_ADVREGEN_1; 		// Enable voltage regulator

	//ADC2 -> CR |= ADC_CR_ADVREGEN_1; 		// Disable voltage regulator
	//ADC2 -> CR &= ADC_CR_ADVREGEN_0; 		// Disable voltage regulator


	ADC2 -> CR |= ADC_CR_ADVREGEN_0;

	HAL_Delay(10);

	ADC2 -> CR &= ~ (ADC_CR_ADCALDIF);
	ADC2 -> CR |= ADC_CR_ADCAL;
	HAL_Delay(10);


	ADC2 -> CR |= ADC_CR_ADEN ; // Enable ADC
	ADC2 -> CR |= ADC_CR_ADSTART;


}




void TIM2_settings(void){

	// Timer used to trigger Start of Conversion for ADC2

	// PA0 -> TIM2 CH1

	RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;

	// TIM2 -> CR1 |= TIM_CR1_CMS_0;		// triangular carrier
	// TIM2 -> CR1 &= ~TIM_CR1_CMS_1;

	TIM2 -> CR1 &= ~( TIM_CR1_CMS_1 | TIM_CR1_CMS_0);

	// TIM2 -> CCMR1 &= ~ TIM_CCMR1_CC1S;
	// TIM2 -> CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	// TIM2 -> CCMR1 &= ~(TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_3);


	//TIM2 -> CCER |= TIM_CCER_CC1E;

	TIM2 -> PSC = 63;
	TIM2 -> ARR = 10000;

	TIM2 -> CR2 &= ~ (TIM_CR2_MMS_0 | TIM_CR2_MMS_2);
	TIM2 -> CR2 |= TIM_CR2_MMS_1;


	// TIM2 -> DIER |= TIM_DIER_UIE;
	// NVIC_EnableIRQ(TIM2_IRQn);

	TIM2 -> CR1 |= TIM_CR1_CEN;



}





#endif /* INC_ADC_SETTINGS_H_ */
