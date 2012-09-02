#pragma config WDT = OFF  
#pragma config OSC = INTIO67  
#pragma config DEBUG = OFF  
#pragma config LVP = OFF
#pragma config PBADEN = OFF
#pragma config BOREN = OFF


#include "p18f4520.h"
//#include "stdio.h"
#include "delays.h"
//#include "usart.h"
//#include "portb.h"


volatile signed int step = 1000;

void motorStep(int nSteps, int sDirection);

void main (void){
	OSCCONbits.IRCF = 7;//Initialize oscillator
	OSCTUNEbits.PLLEN = 1;

	while(!OSCCONbits.IOFS);//Wait
	TRISB = 0;
    TRISD = 0; 
	INTCON2bits.RBPU = 0;		// enable PORTB internal pullups
    //ADCON2 = 0b00111000;
   	//ADCON0 = 0b00000001;
	/*TRISD = 0x00;
	TRISCbits.TRISC6 = 1;//initializing PORTC
	TRISCbits.TRISC7 = 1;  
	RCSTAbits.SPEN = 1;//Enable USART      
	TRISCbits.TRISC0 = 0;

	OpenUSART(USART_TX_INT_OFF &//USART config  
	USART_RX_INT_OFF &  
	USART_ADDEN_OFF &   
	USART_ASYNCH_MODE &   
	USART_EIGHT_BIT &  
	USART_CONT_RX &   
	USART_BRGH_LOW, 8); //57.6k baud rate at 32MHz
*/
	while(1){
		motorStep(200,1);
		motorStep(200,0);
		//Pulse

	}
}

void motorStep(int nSteps, int sDirection){
	while(nSteps > 0){
		if(sDirection == 0){
			step--;
		}
		else{
			step++;
		}
		switch(step % 4){
			case 0:
				LATD = 0b10100000;
			break;
			case 1:
				LATD = 0b01100000;			
			break;
			case 2:
				LATD = 0b01010000;			
			break;
			case 3:
				LATD = 0b10010000;			
			break;
		}
		nSteps--;
		
		Delay10KTCYx(30);
	}
}

