
#include "p18f4520.h"
#include "delays.h"
#include "usart.h"


#pragma config WDT = OFF  
#pragma config OSC = INTIO67  
#pragma config DEBUG = ON  
#pragma config LVP = OFF



#pragma interrupt handleInterrupt  // "interrupt" pragma also for high priority
void handleInterrupt(void){
	int i = 0;
}

#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh (void){
  _asm
    goto handleInterrupt //jump to interrupt routine
  _endasm
}

void main (void){
	OSCCONbits.IRCF = 7;  
	OSCTUNEbits.PLLEN = 1;

	while(!OSCCONbits.IOFS);//Wait

	TRISCbits.TRISC6 = 1;     //initializing port as per datasheet p201  
	TRISCbits.TRISC7 = 1;  
	RCSTAbits.SPEN = 1;      
	TRISCbits.TRISC0 = 0;

	OpenUSART(USART_TX_INT_OFF &         //USART config  
	USART_RX_INT_OFF &  
	USART_ADDEN_OFF &   
	USART_ASYNCH_MODE &   
	USART_EIGHT_BIT &  
	USART_CONT_RX &   
	USART_BRGH_LOW, 8); 

	//Timer config
	
	//Interrupt0 config
	
		
	while(1){    
		if(BusyUSART() == 0 ){    
			putrsUSART("Kitty cat ");   
		}  

	}    


}