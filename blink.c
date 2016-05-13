#include"p18f4520.h"
#include "stdio.h"
#include "LCD.h"
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "Delay.h"
#include"USART.h"
#include "string.h"

extern BYTE LCDText[16*2+1];
unsigned long int pressure[2], gas[2], average, results;
unsigned char temp[4];
unsigned int i;

void alert_enable();
void alert_disable();
void motor_enable();
void motor_disable();
unsigned long int gas_sensor();
unsigned long int pressure_sensor();
void inttostring(long int a);
void send_msg();

void inttostring(long int a)
{
	for(i=0;i<4;i++) {
		temp[i] = inttochar(a%10);
		a=a/10;
	}
}
// read gas sensor value
unsigned long int gas_sensor() {
	unsigned long int result=0;
	ADCON0 = 0x00;
	ADCON0bits.ADON = 1;
	ADCON0bits.GO = 1;
	while(ADCON0bits.DONE == 1);
	ADCON0bits.ADON = 0;
	result = (ADRESH<<8) | ADRESL;
	return((result*5000UL)/1024);
}

// read pressure sensor value
unsigned long int pressure_sensor() {
	unsigned long int result=0;
	ADCON0 = 0x04;
	ADCON0bits.ADON = 1;
	ADCON0bits.GO = 1;
	while(ADCON0bits.DONE == 1);
	ADCON0bits.ADON = 0;
	result = (ADRESH<<8) | ADRESL;
	return((result*5000UL)/1024);
}

// enable buzzer sound and led indication
void alert_enable()
{
	//PORTB |= 0x03;

	PORTBbits.RB7 = 1;
	DelayMs(100);
	PORTBbits.RB7 = 0;
	PORTB &= 0xfc;
}

void motor_enable()
{
	PORTBbits.RB6 = 1;
	DelayMs(250);
	PORTBbits.RB6 = 0;
	PORTB &= 0xfc;
}

//	disable buzzer and led indication
void alert_disable()
{
	PORTBbits.RB7 = 0;
}

void motor_disable() {
	PORTBbits.RB6 = 0;
}

void normal()
{
		strcpypgm2ram((char*)LCDText,"CONDITION");
		display_row(0);
		DelayMs(2);
		strcpypgm2ram((char*)LCDText, "NORMAL!");
		display_row(1);
		alert_disable();	
}

void send_msg() {

printf("AT");
transmit_fxn(0x0D);
transmit_fxn(0x0A);
DelayMs(500);

printf("AT+CMGS="\"9566786199\"");
transmit_fxn(0x0D);
DelayMs(500);

printf("PRESSURE LOW");
transmit_fxn(0x1A);
transmit_fxn(0x0D);
DelayMs(500);

}
void main(void)
{

	LCDinit();
	TRISCbits.TRISC7=1;
	TRISCbits.TRISC6=1;
	InitUSART();
	DelayMs(2);
	strcpypgm2ram((char*)LCDText,"WELCOME");
	display_row(0);
	DelayMs(2);
	strcpypgm2ram((char*)LCDText, "HAPPY HEALTH");
	display_row(1);

	TRISB = 0x0f;		// POPORTDbits.RTB as Input
	PORTB = 0x00;
	LATB = 0x00;
	ADCON1 = 0x0d;
	ADCON2 = 0xbe; 		// left justified and Time Acquisition as 2
	TRISA = 0xe1;		// POPORTDbits.RTA as Output
	PORTA = 0x1e;
	LATA = 0x1e;
	DelayMs(500);
	lcd_cmd(0x01);
	gas_sensor();
	gas_sensor();
	DelayMs(50);
			//Training mode at initial stage
			strcpypgm2ram((char*)LCDText,"PLEASE WAIT!");
			display_row(0);
			DelayMs(2);
			strcpypgm2ram((char*)LCDText, "TESTING...");
			display_row(1);
			DelayMs(500);
			pressure[0] = pressure_sensor();
			only_4_per_row (1, pressure[0]);
			DelayMs(500);
			pressure[1] = pressure_sensor();
			only_4_per_row (1, pressure[1]);
			DelayMs(500);
			gas[0] = gas_sensor();
			only_4_per_row (1, gas[0]);
			DelayMs(500);
			gas[1] = gas_sensor();
			only_4_per_row (1, gas[1]);
			normal();
			DelayMs(3000);

	while(1)
		{
			average=(gas[0]+gas[1])/2;
			results=0;
			DelayMs(500);
			results = gas_sensor();
			only_4_per_row (1, results);
			DelayMs(500);
			only_4_per_row (1, average);
			if((average) < results) {
				strcpypgm2ram((char*)LCDText,"ABNORMAL EVT");
				display_row(0);
				DelayMs(2);
				alert_enable();
				alert_disable();
			}
			else {
				normal();
			}

			results=0;
			DelayMs(500);
			results = pressure_sensor();
			average=(pressure[0]+pressure[1])/2;
			only_4_per_row (1, results);
			DelayMs(500);
			only_4_per_row (1, average);
			DelayMs(500);
			if((average+15) <(results+0)) {
				strcpypgm2ram((char*)LCDText,"PRESSURE LOW");
				display_row(0);
				DelayMs(2);
				send_msg();
				alert_enable();
				alert_disable();
				for(i=0;i<2;i++) { // enable motor for 5 times when pressure get low
					motor_enable();
					DelayMs(50);
					motor_disable();
				}
				send_msg();
			}
			else {
				normal();
			}
			
	}
}