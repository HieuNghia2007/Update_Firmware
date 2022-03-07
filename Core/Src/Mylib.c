/*
 * Mylib.c
 *
 *  Created on: Mar 7, 2022
 *      Author: Tran Le Hieu Nghia
 */


/* Using for STM32F407VG */

/* Include */
#include "stm32f4xx_hal.h"
#include "Mylib.h"

/* Function */
void DMA2_Stream2_Clear_InterruptFlag()
{
	uint32_t* DMA2_LIFCR = (uint32_t*)(DMA2_BASE_ADD + 0x08);
	*DMA2_LIFCR |= 0b11111 << 18;
}

/* Stream number (0-7) and Channel selection (0-7) */
void DMA2_Init(int Stream, int Channel, uint32_t ScrAddr, uint32_t DstAddr, uint32_t DataLength)
{
	__HAL_RCC_DMA2_CLK_ENABLE();

	uint32_t* DMA_SxCR = (uint32_t*)(DMA2_BASE_ADD + 0x10 + 0x18 * Stream);
	uint32_t* DMA_SxNDTR = (uint32_t*)(DMA2_BASE_ADD + 0x14 + 0x18 * Stream);
	uint32_t* DMA_SxPAR = (uint32_t*)(DMA2_BASE_ADD + 0x18 + 0x18 * Stream);
	uint32_t* DMA_SxM0AR = (uint32_t*)(DMA2_BASE_ADD + 0x1c + 0x18 * Stream);

	/* Data transfer direction: Peripheral to memory */

	/* Disable Stream before Configure */
	*DMA_SxCR &= ~1;

	/* Configure DMA stream x: Select channel, Memory increment mode, Circular mode enable */
	*DMA_SxCR |= (Channel << 25) | (1 << 10) | (1 << 8);

	/* Set Number of data for DMA stream x */
	*DMA_SxNDTR = DataLength;

	/* Set Peripheral address for DMA stream x */
	*DMA_SxPAR = (uint32_t)ScrAddr;

	/* Set Memory 0 address for DMA stream x */
	*DMA_SxM0AR = (uint32_t)DstAddr;

	/* Enable Stream */
	*DMA_SxCR |= 1;
}

/* Stream number (0-7) and Channel selection (0-7) */
void DMA2_Init_IT(int Stream, int Channel, uint32_t ScrAddr, uint32_t DstAddr, uint32_t DataLength)
{
	__HAL_RCC_DMA2_CLK_ENABLE();

	uint32_t* DMA_SxCR = (uint32_t*)(DMA2_BASE_ADD + 0x10 + 0x18 * Stream);
	uint32_t* DMA_SxNDTR = (uint32_t*)(DMA2_BASE_ADD + 0x14 + 0x18 * Stream);
	uint32_t* DMA_SxPAR = (uint32_t*)(DMA2_BASE_ADD + 0x18 + 0x18 * Stream);
	uint32_t* DMA_SxM0AR = (uint32_t*)(DMA2_BASE_ADD + 0x1c + 0x18 * Stream);

	/* Data transfer direction: Peripheral to memory */

	/* Disable Stream before Configure */
	*DMA_SxCR &= ~1;

	/* Configure DMA stream x: Select channel, Memory increment mode, Circular mode enable */
	*DMA_SxCR |= (Channel << 25) | (1 << 10) | (1 << 8);

	/* Set Number of data for DMA stream x */
	*DMA_SxNDTR = DataLength;

	/* Set Peripheral address for DMA stream x */
	*DMA_SxPAR = (uint32_t)ScrAddr;

	/* Set Memory 0 address for DMA stream x */
	*DMA_SxM0AR = (uint32_t)DstAddr;

	/* Clear all Interrupt flag before enable interrupt */
	DMA2_Stream2_Clear_InterruptFlag();

	/* Enable interrupt DMA2 transfer complete*/
	*DMA_SxCR |= 1 << 4;

	/* Enable stream */
	*DMA_SxCR |= 1;

	uint32_t* NVIC_ISER1 = (uint32_t*)0xe000e104;

	/* NVIC: Register set-enable Interrupt DMA2_Stream2 */
	*NVIC_ISER1 |= (1 << 26);
}

void UART6_Init()
{
	/* Enable CLK GPIOC */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	uint32_t* GPIOC_MODER = (uint32_t*)GPIOC_BASE_ADD;
	uint32_t* GPIOC_AFRL = (uint32_t*)(GPIOC_BASE_ADD + 0x20);

	/* Set mode: Alternate function mode PC6 and PC7 */
	*GPIOC_MODER |= (0b10 << 12) | (0b10 << 14);

	/* Select Alternate function: PC6 UART6_Tx, PC7 UART6_Rx */
	*GPIOC_AFRL |= (0b1000 << 24) | (0b1000 << 28);

	/* Enable CLK UART6 */
	__HAL_RCC_USART6_CLK_ENABLE();

	uint32_t* BRR = (uint32_t*)(UART6_BASE_ADD + 0x08);
	uint32_t* CR1 = (uint32_t*)(UART6_BASE_ADD + 0x0c);

	/* Configure UART6: UART enable, Word length 1 Start bit - 8 Data bits, Transmitter enable, Receiver enable */
	*CR1 |= (1<< 13) | (1<< 3) | (1<< 2);

	/* Set Baud rate: 9600 */
	/* Fuart = 16Mhz
	 * Baud rate = 9600
	 * 16 000 000 / (16 * 9600) = 104.1667
	 * DIV_Mantissa = 104
	 * DIV_Fraction = 0.1667 * 16 ~ 3
	 * */
	*BRR = (104 << 4) | 3;
}

void UART6_Send(char data)
{
	uint16_t* UART6_SR = (uint16_t*)(UART6_BASE_ADD + 0x00);
	uint16_t* UART6_DR = (uint16_t*)(UART6_BASE_ADD + 0x04);
	/* Wait TXE: Transmit data register empty */
	while(((*UART6_SR >> 7) & 1) != 1);

	/* Send Data */
	*UART6_DR = data;

	/* Wait TC: Transmission complete */
	while(((*UART6_SR >> 6) & 1) != 1);

	/* Clear TC bit before continue send next data */
	*UART6_SR &= ~(1<<6);
}

void UART6_Transmit(char* pData, int size)
{
	int i = 0;
	while (size > 0)
	{
		UART6_Send(pData[i++]);
		size--;
	}

}

void UART6_DMA_Stop()
{
	uint32_t* CR3 = (uint32_t*)(UART6_BASE_ADD + 0x14);
	*CR3 &= ~(1 << 6);
}

void UART6_DMA_Start()
{
	uint32_t* CR3 = (uint32_t*)(UART6_BASE_ADD + 0x14);
	*CR3 |= (1 << 6);
}

void UART6_ClearOREFlag()
{
	uint16_t* UART6_SR = (uint16_t*)(UART6_BASE_ADD + 0x00);
	uint16_t* UART6_DR = (uint16_t*)(UART6_BASE_ADD + 0x04);
	uint16_t tmp;
	tmp = *UART6_SR;
	tmp = *UART6_DR;
}

void UART6_Receive_DMA(uint32_t ScrAddr, uint32_t DstAddr, uint32_t DataLength)
{
	DMA2_Init(2, 5, ScrAddr, DstAddr, DataLength);
	UART6_ClearOREFlag();
	UART6_DMA_Start();
}

void UART6_Receive_DMA_IT(uint32_t ScrAddr, uint32_t DstAddr, uint32_t DataLength)
{
	DMA2_Init_IT(2, 5, ScrAddr, DstAddr, DataLength);
	UART6_ClearOREFlag();
	UART6_DMA_Start();
}

__attribute__((section (".RamFunc"))) void FLASH_erase(char sector)
{
	uint32_t* KEYR = (uint32_t)(FLASH_BASE_ADD + 0x04);
	uint32_t* SR = (uint32_t)(FLASH_BASE_ADD + 0x0C);
	uint32_t* CR = (uint32_t)(FLASH_BASE_ADD + 0x10);

	/* Check LOCK */
	if(((*CR >> 31) & 1) == 1)
	{
		/* If locked, unlock by Key */
		*KEYR = 0x45670123;
		*KEYR = 0xCDEF89AB;
	}

	/* Wait bit BSY: not busy */
	while(((*SR >> 16) & 1) == 1);

	/* Set bit SER: Sector Erase */
	*CR |= 1 << 1;

	/* Select Sector number */
	*CR |= sector << 3;

	/* Set bit STRT: Start */
	*CR |= 1 << 16;

	/* Wait bit BSY: not busy */
	while(((*SR >> 16) & 1) == 1);

	/* Reset bit SER */
	*CR &= ~(1 << 1);
}

__attribute__((section (".RamFunc"))) void FLASH_program(char* addr, char data)
{
	uint32_t* KEYR = (uint32_t)(FLASH_BASE_ADD + 0x04);
	uint32_t* SR = (uint32_t)(FLASH_BASE_ADD + 0x0C);
	uint32_t* CR = (uint32_t)(FLASH_BASE_ADD + 0x10);

	/* Check LOCK */
	if(((*CR >> 31) & 1) == 1)
	{
		/* If locked, unlock by Key */
		*KEYR = 0x45670123;
		*KEYR = 0xCDEF89AB;
	}

	/* Wait BSY: busy bit */
	while(((*SR >> 16) & 1) == 1);

	/* Set bit PG: Programming */
	*CR |= 1;

	/* Write data */
	*addr = data;

	/* Wait BSY: busy bit */
	while(((*SR >> 16) & 1) == 1);

	/* Reset bit PG */
	*CR = 0;
}

__attribute__((section (".RamFunc"))) void Update(int size, char* pData)
{
	  FLASH_erase(0);

	  /* Pointer to Sector 0 */
	  char* start_program = 0x08000000;
	  for(int i = 0; i < size; i++)
	  {
		  FLASH_program(start_program + i, pData[i]);
	  }

	  /* Software reset */
	  uint32_t* AIRCR = (uint32_t*)(0xE000ED0C);
	  *AIRCR |= (0x5FA << 16) | (1<<2); // SYSRESETREQ
}
