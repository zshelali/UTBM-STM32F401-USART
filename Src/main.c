/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Ali Hajeri
 * @brief          : Main program body,
 * 					 simple application to test and debug
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"






int __io_putchar(int ch){
	while ((USART2->SR & (1 << 7)) == 0 ); //waiting until TXE is set
	USART2->DR = ch; //writing a char to data register
	return ch;
}

void USART2_Transmit(uint8_t* data, uint32_t len){
	int count = 0;
	do {
		__io_putchar(*data++);
		count++;
		} while(count < len);//repeat transmission for each character in the string

}


int __io_getchar(void){
	while ((USART2->SR & (1 << 5)) == 0); //waiting until the RXNE flag is set
	int ch = USART2->DR;
	return ch;
}

int32_t USART2_Receive(uint8_t* data, uint32_t len, uint32_t timeout){
	int count = 0; //char counter
	int timer = 0; //timer for the timeout management
    
    /*wait for the first data byte using timeout*/
	while ((USART2->SR & (1 << 5)) == 0){
		if (timer++ >= timeout){
			return -1; //TIMEOUT
		}
	}
	*data = __io_getchar();
	count++;
	data++; //move to next char

	timer = 0; //reset the timeout timer
    
    /* remaining data*/
	do {
		while ((USART2->SR & (1 << 5)) == 0){
				if (timer++ >= timeout){
					return count;
				}
			}
		timer = 0;
		*data = __io_getchar();
		count++;
		len--;
		data++;
	} while (len > 0);


	return count;

}


int main(void)
{
	/*  GPIOA and USART22 clock activation*/
	RCC->AHB1ENR |= 1;
	RCC->APB1ENR |= 0x20000;

	/* 2 pins declaration */
	GPIO_TypeDef* PA2 = GPIOA;
	GPIO_TypeDef* PA3 = GPIOA;

	/*reset pin modes*/
	PA2->MODER &= ~(3 << 4);
	PA3->MODER &= ~(3 << 6);

	/* AF7 mode */
	PA2->MODER |= (2 << 4);
	PA2->AFR[0] |= (7 << 8);
	PA3->MODER |= (2 << 6);
	PA3->AFR[0] |= (7 << 12);

	/* 1 Stop bit */
	USART2->CR2 &= ~(3 << 12);

	/* baud rate configuration (speed)*/
	USART2->BRR = 16000000 / 115200;

	/*enable USART2 transmission, reception, 8 data bits and
	 *16x oversampling mode
	 */
	USART2->CR1 |= (1 << 2) | (1 << 3) | (1 << 13); //RE,TE,USART2 --> ON
	USART2->CR1 &= ~(1 << 15); //OVERSAMPLING by 16
	USART2->CR1 &= ~(1 << 12); //Word length 8 bits
	USART2->CR1 &= ~(3 << 9); //Parity and PCE disabled


	uint8_t buffer[100];
	int32_t received_data;
	while(1){

		/* TEST RECEIVE */
		received_data = USART2_Receive(buffer, sizeof(buffer), 50000);
		    if (received_data > 0) {
		        USART2_Transmit(buffer, received_data);
		    }


		 /* TEST TRANSMIT */


		// USART2_Transmit("Tulipe", 6);
		// for (int i = 0; i < 1000000 * 5; i++); //5s delay


	}

	return 0;
}











