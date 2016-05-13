#include "p18f4520.h"
#include "stdio.h"
#include "Delay.h"
#include "USART.h"
#include "LCD.h"

unsigned char bus_reg[] = "TN31J6030";

void InitUSART()
{
	TXSTA = 0x20;
	BAUDCONbits.BRG16 =0;
	SPBRG = 31;			// 9600 Baud Rate
	TXSTAbits.TXEN = 1;
	RCSTAbits.SPEN =1;
	RCSTAbits.CREN =1;
}

unsigned char getchar(void)
{
	unsigned char ch;
	//wait until there's a character to be read
	while (!(PIR1bits.RCIF==1));
	//then read from the Receiver Buffer Register
	ch=RCREG;
	return ch;
}

unsigned char transmit_fxn(unsigned char ch)
{
	while (!(PIR1bits.TXIF==1));
	TXREG=ch;
	return ch;
}

void serial_comm(unsigned int count_cpy,unsigned int amount_cpy)
{
	unsigned int count_length,amount_length,length_cpy,mx,j;
	unsigned char a[20],string_count[20],string_amount[20];
	count_length=10;
	amount_length=10;
		for(j=0;j<=count_length;j++) //count string
			{
			mx=count_cpy%10;
			a[j]=inttochar(mx);
			count_cpy = count_cpy/10;
			}
		length_cpy=count_length;
		for(j=0;j<=count_length;j++)
			{
			string_count[j]=a[length_cpy-1];
			length_cpy=length_cpy-1;
			} 
		for(j=0;j<=amount_length;j++) //amount string
			{
			mx=amount_cpy%10;
			a[j]=inttochar(mx);
			amount_cpy = amount_cpy/10;
			}
		length_cpy=amount_length;
		for(j=0;j<=amount_length;j++)
			{
			string_amount[j]=a[length_cpy-1];
			length_cpy=length_cpy-1;
			}
		transmit_fxn('<');
			for(j=0;j<9;j++)
			{
				transmit_fxn(bus_reg[j]);
			}
		transmit_fxn('>');
		for (j=0;j<=count_length;j++)
		{
			transmit_fxn(string_count[j]);
		}
		transmit_fxn('/');
		for (j=0;j<=amount_length;j++)
		{
			transmit_fxn(string_amount[j]);
		}
		transmit_fxn('=');
}