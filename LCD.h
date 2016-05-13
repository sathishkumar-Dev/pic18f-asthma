#ifndef _LCD_H
#define _LCD_H
void display_row (unsigned char p_byRow);
void display_row_var (unsigned char p_byRow, unsigned char str[16]);

void LCDinit(void);
unsigned char inttochar(int m);
unsigned int print_ticket (unsigned int source,unsigned int destination);
void only_4_per_row (unsigned int row,unsigned long int result);
extern unsigned int lengths(unsigned int n);
#endif