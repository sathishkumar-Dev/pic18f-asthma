#ifndef _USART_H
#define _USART_H
void InitUSART();
unsigned char transmit_fxn(unsigned char ch);
unsigned char getchar(void);
void serial_comm(unsigned int count_cpy,unsigned int amount_cpy);

#endif