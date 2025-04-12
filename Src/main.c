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
	while ((USART2->SR & (1 << 7)) == 0 ); //attendre que le flag TXE passe a 1
	USART2->DR = ch; //ecriture d'un char dans ch
	return ch;
}

void USART2_Transmit(uint8_t* data, uint32_t len){
	int count = 0;
	do {
		__io_putchar(*data++);
		count++;
		} while(count < len);//repeter pour tous les caracteres de la chaine

}


int __io_getchar(void){
	while ((USART2->SR & (1 << 5)) == 0); //attendre que le flag RXNE passe a 1
	int ch = USART2->DR;
	return ch;
}

int32_t USART2_Receive(uint8_t* data, uint32_t len, uint32_t timeout){
	int count = 0; //compteur de caracteres
	int timer = 0; //timer pour la gestion du timeout
    
    /*premiere donnee*/
	while ((USART2->SR & (1 << 5)) == 0){ // attente du RXNE
		if (timer++ >= timeout){
			return -1;
		}
	}
	*data = __io_getchar();
	count++;
	data++; //passage à la donnée suivante

	timer = 0; //timer du timeout remis a 0
    
    /* reste des donnees*/
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
	/* Activation de la clock de GPIOA et USART22*/
	RCC->AHB1ENR |= 1;
	RCC->APB1ENR |= 0x20000;

	/* Declaration des deux pins */
	GPIO_TypeDef* PA2 = GPIOA;
	GPIO_TypeDef* PA3 = GPIOA;

	/*reinitialisation des pins*/
	PA2->MODER &= ~(3 << 4);
	PA3->MODER &= ~(3 << 6);

	/* mode AF7 */
	PA2->MODER |= (2 << 4);
	PA2->AFR[0] |= (7 << 8);
	PA3->MODER |= (2 << 6);
	PA3->AFR[0] |= (7 << 12);

	/* 1 Stop bit */
	USART2->CR2 &= ~(3 << 12);

	/* configuration vitesse */
	USART2->BRR = 16000000 / 115200;

	/*activation emission, reception et peripherique avec 8 bits de donnees et
	 * un oversampling de 16
	 */
	USART2->CR1 |= (1 << 2) | (1 << 3) | (1 << 13); //RE,TE,USART2 --> ON
	USART2->CR1 &= ~(1 << 15); //OVER by 16
	USART2->CR1 &= ~(1 << 12); //Word length 8 bits
	USART2->CR1 &= ~(3 << 9); //Parite et PCE 0


	uint8_t buffer[100];
	int32_t received_data;
	/* entree dans la boucle infinie */
	while(1){

		/* TEST RECEIVE */
		received_data = USART2_Receive(buffer, sizeof(buffer), 50000);
		    if (received_data > 0) {
		        USART2_Transmit(buffer, received_data);
		    }


		 /* TEST TRANSMIT */


		// USART2_Transmit("Tulipe", 6);
		// for (int i = 0; i < 1000000 * 5; i++); //attente de 5 secondes


	}

	return 0;
}











