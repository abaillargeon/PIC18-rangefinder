#pragma config WDT = OFF  
#pragma config OSC = INTIO67  
#pragma config DEBUG = OFF  
#pragma config LVP = OFF
#pragma config PBADEN = OFF

#include "p18f4520.h"
#include "stdio.h"
#include "delays.h"
#include "usart.h"
#include "portb.h"

//rom char mystring[] = "Send me to the USART";
volatile int timerCounter = 0;
volatile int seconds = 0;
volatile char waiting = 1;
volatile signed int step = 0;

void InterruptServiceLow(void);
void InterruptServiceHigh(void);
void motorStep(int nSteps, int direction);

#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh (void){
  _asm
	goto InterruptServiceHigh //jump to interrupt routine
  _endasm
}

#pragma code InterruptVectorLow = 0x18
void InterruptVectorLow (void){
  _asm
	goto InterruptServiceLow //jump to interrupt routine
  _endasm
}

void main (void){
	OSCCONbits.IRCF = 7;//Initialize oscillator
	OSCTUNEbits.PLLEN = 1;

	while(!OSCCONbits.IOFS);//Wait
	
	TRISBbits.TRISB0 = 1; 
	INTCON2bits.RBPU = 0;// enable PORTB internal pullups
	ADCON2 = 0b00111000;
	ADCON0 = 0b00000001;

	TRISD = 0x00;
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

	//Timer0 config
	INTCONbits.TMR0IF = 0;// clear roll-over interrupt flag
	INTCON2bits.TMR0IP = 0;// Timer0 is low priority interrupt
	INTCONbits.TMR0IE = 1;// enable the Timer0 interrupt.
	T0CON = 0b01001000;// prescaler 1:1
	TMR0H = 0;// clear timer - always write upper byte first
	TMR0L = 23;//32MHz/(4*256-23)
	T0CONbits.TMR0ON = 0;// start timer	

	INTCON2bits.INTEDG0 = 0;    // interrupt on falling edge of INT0 (switch pressed)
	INTCONbits.INT0IF = 0;      // ensure flag is cleared
	INTCONbits.INT0IE = 0;      // disable INT0 interrupt

	RCONbits.IPEN = 1;// enable priorities on interrups
	INTCONbits.GIEL = 1;// enable low priority interrupts
	INTCONbits.GIEH = 1;// enable high priority interrupts
		
	while(1){
		//Pulse
		waiting = 1;
		timerCounter = 0;
		LATDbits.LATD2 = 1;
		//Start timer0
		T0CONbits.TMR0ON = 1;
		Delay100TCYx(100);
		LATDbits.LATD2 = 0;
		//Delay feedback - .002s*32Mhz/4 
		Delay1KTCYx(22);
		//Enable INT0
		INTCONbits.INT0IF = 0;
		INTCONbits.INT0IE = 1;
		//Wait for echo...
		while(waiting && timerCounter < 5000);
		//Output step/distance
		printf("%d,%d\r\n",step,timerCounter/2);
		//Next step
		motorStep(1,1);
		if(step > 50){
			motorStep(50,0);
		}
	}
}

void motorStep(int nSteps, int direction){
	while(nSteps > 0){
		if(direction == 0){
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

#pragma interrupt InterruptServiceHigh  // "interrupt" pragma also for high priority
void InterruptServiceHigh(void){
	// Check for INT0 interrupt
	//waiting = 0;
	if (INTCONbits.INT0IF){
		// clear (reset) flag
		T0CONbits.TMR0ON = 0;//Disable timer
		waiting = 0;
		//LATDbits.LATD2 = 1;
		INTCONbits.INT0IF = 0;
		INTCONbits.INT0IE = 0;      
	}
} 

#pragma interruptlow InterruptServiceLow//low priority
void InterruptServiceLow(void){    
	if  (INTCONbits.TMR0IF){//Check timer0 Interrupt
		timerCounter++;
		INTCONbits.TMR0IF = 0;// clear (reset) flag
		TMR0H = 0;      
		TMR0L = 23;         
	}
}