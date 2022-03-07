/*
 * Mylib.h
 *
 *  Created on: Mar 7, 2022
 *      Author: Tran Le Hieu Nghia
 */

#ifndef INC_MYLIB_H_
#define INC_MYLIB_H_



#endif /* INC_MYLIB_H_ */

/* Define */
#define GPIOC_BASE_ADD 0x40020800
#define UART6_BASE_ADD 0x40011400
#define DMA2_BASE_ADD 0x40026400
#define FLASH_BASE_ADD 0x40023c00

/* Function */
void DMA2_Stream2_Clear_InterruptFlag();
void DMA2_Init(int Stream, int Channel, uint32_t ScrAddr, uint32_t DstAddr, uint32_t DataLength);
void DMA2_Init_IT(int Stream, int Channel, uint32_t ScrAddr, uint32_t DstAddr, uint32_t DataLength);
void UART6_Init();
void UART6_Send(char data);
void UART6_Transmit(char* pData, int size);
void UART6_DMA_Stop();
void UART6_DMA_Start();
void UART6_ClearOREFlag();
void UART6_Receive_DMA(uint32_t ScrAddr, uint32_t DstAddr, uint32_t DataLength);
void UART6_Receive_DMA_IT(uint32_t ScrAddr, uint32_t DstAddr, uint32_t DataLength);
__attribute__((section (".RamFunc"))) void FLASH_erase(char sector);
__attribute__((section (".RamFunc"))) void FLASH_program(char* addr, char data);
__attribute__((section (".RamFunc"))) void Update();
