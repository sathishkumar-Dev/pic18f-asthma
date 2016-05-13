#include "p18f4520.h"
#include "stdio.h"
#include "Delay.h"
#include "LCD.h"
#include "GenericTypeDefs.h"
#include "Compiler.h"

BYTE LCDText[16*2+1];
void delay_loc();
void nop ();
void delay();
void delay_100us();


/*************************************************************
            100 micro sec delay loop
************************************************************/
void delay_loc()
	{
		int i,j;
		for (i = 0; i < 500; i++)
			{
				for (j = 0; j < 100; j++);
			}
	}

void delay_100us()
{
   int i;
   for (i = 0; i < 18; i++);
}

/*************************************************************
            Variable delay in ms
*************************************************************/
void delay()
{
   int i;
   for (i = 0; i < 800; i++);
}

void nop ()
{}

void lcd_cmd1(unsigned char data)
{
   LATE = 0x00;     				// RS = 0;  // RW = 0;
   LATD = (data & 0xf0);    		// Write Higher Nibble to PORTD (LATD)
   LATE |= 0x01;   					// LCD_EN = 1;
   delay();
   LATE &= 0x00;   					// LCD_EN = 0;
}

void lcd_dat1(unsigned char data)
{
	LATE = 0x04; 					// RS = 1;  // RW = 0;
   	LATD = (data & 0xf0);			// Write Higher Nibble to PORTD (LATD)
   	LATE |= 0x01; 					// LCD_EN = 1;
    delay();
    LATE &= 0x04; // LCD_EN = 0;
}

void lcd_cmd(unsigned char cmd)
{
	// sends command to LCD
	lcd_cmd1(cmd & 0xf0) ;
	lcd_cmd1(cmd << 4) ;
}

void lcd_dat (unsigned char dat)
{
	// sends data to LCD
	lcd_dat1(dat & 0xf0) ;
	lcd_dat1(dat << 4) ;
}


void LCDinit(void)
{
   	PORTE = 00;         // Clear Control port
   	LATE = 00;         	// and its corresponding Latch
   	TRISE = 00;        	// Make PORTE as output port
	TRISD &= 0x0F;  	// Make output;
   	delay();         
   	delay();          
   	delay();         	//~15 ms delay

   	lcd_cmd1(0x30);  	// #1 control byte
   	delay();
   	lcd_cmd1(0x30);  	// #2 control byte
   	delay_100us();
   	lcd_cmd1(0x30);  	// #3 control byte
   	delay_100us();
   	lcd_cmd1(0x20);  	// #4 control byte, sets 4 bit mode
   	delay_100us();

   	lcd_cmd(0x28);   	// #5 control byte Function set
   	lcd_cmd(0x0D);    	// Turn on Display
   	lcd_cmd(0x01);    	// Clear Display (clears junk if any)
	lcd_cmd(0x06);   	// Entry mode selection
}                  

void display_row (unsigned char p_byRow)
{
	unsigned char i,j;
	unsigned char k;
	if(p_byRow == 1)			// if row  is same as saying if row is non zero, in our case its row = 1
	{
	// Set the address to the second line
	lcd_cmd(0xC0);
	Delay10us(5);

	// Output second line
		for(i = 0; i < 16u; i++)
		{
		// Erase the rest of the line if a null char is 
		// encountered (good for printing strings directly)
		if(LCDText[i] == 0u)
		{
			for(j=i; j < 16u; j++)
			{
				LCDText[j] = ' ';
			}
		}
		
		lcd_dat(LCDText[i]);
		Delay10us(5);
		}
	
	}

	else
	{
		lcd_cmd(0x80);
		DelayMs(2);

		// Output first line
		for(i = 0; i < 16u; i++)
		{
		// Erase the rest of the line if a null char is 
		// encountered (good for printing strings directly)
		if(LCDText[i] == 0u)
		{
			for(j=i; j < 16u; j++)
			{
				LCDText[j] = ' ';
			}
		}
		lcd_dat(LCDText[i]);
		Delay10us(5);
		
		}
	}
}

void display_row_var (unsigned char p_byRow, unsigned char str[16])
{
	unsigned char k,cpy_str[2];
	unsigned long int i = 0;
	if(p_byRow == 1)			// if row  is same as saying if row is non zero, in our case its row = 1
	{
		lcd_cmd(0xc0);
	}
	else
	{
		lcd_cmd(0x80);
	}
		
	for(k = 0 ; k < 16 ; k ++)
	{

		{
			if (str[k])
				lcd_dat(str[k]) ;
			else
				break ;
		}
	}
	while(k < 16)
	{
		lcd_dat(' ') ;
		k ++ ;
	}

}	

unsigned char inttochar(int m)
{
char a;
a= m+0x30;
return(a);
}

/***************************************************************
						ticket printing
/****************************************************************/

unsigned int print_ticket(unsigned int source,unsigned int destination)
{
	unsigned int final_amount =0,final_amount_cpy =0,fx =0,final_length_cpy =0,j,final_amount_length =0;
	unsigned char final_disp[20],afx[5];
	if(source > destination)
	{
		final_amount = (source - destination)*5;
	}
	if(destination>source)
	{
		final_amount = (destination - source)*5;
	}	
	final_amount_cpy = final_amount;
	final_disp[0] = 'R';
	final_disp[1] = 's';
	final_disp[2] = ':';
	final_disp[3] = '1';
	final_disp[4] = '*';
	final_amount_length=10;
		for(j=0;j<=final_amount_length;j++)
			{
			fx=final_amount_cpy%10;
			afx[j]=inttochar(fx);
			final_amount_cpy = final_amount_cpy/10;
			}
		final_length_cpy=final_amount_length;
		for(j=0;j<=final_amount_length;j++)
			{
			final_disp[j+5]=afx[final_length_cpy-1];
			final_length_cpy=final_length_cpy-1;
			}
	final_disp[final_amount_length+5] = '=';
	final_length_cpy=final_amount_length;
		for(j=0;j<=final_amount_length;j++)
		{
			final_disp[final_amount_length+6+j]=afx[final_length_cpy-1];
			final_length_cpy=final_length_cpy-1;
		}
	DelayMs(2);
	strcpypgm2ram((char*)LCDText, final_disp);
	display_row(1);
	//display_row(1,final_disp);
	return final_amount;
}

void only_4_per_row (unsigned int p_byRow,unsigned long int result) {
	int i;
	if(p_byRow == 1)			// if row  is same as saying if row is non zero, in our case its row = 1
	{
		lcd_cmd(0xc0);
	}
	else
	{
		lcd_cmd(0x80);
	}
	for(i=0;i<4;i++) {
	}
	Delay10us(5);
	lcd_dat(inttochar((result/1000)%10));
	lcd_dat(inttochar((result/100)%10));
	lcd_dat(inttochar((result/10)%10));
	lcd_dat(inttochar(result%10));	
	for(i=0;i<12;i++) {
		lcd_dat(' ');
	}
}