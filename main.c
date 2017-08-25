/*
 * 2P-1D-PONG.c
 *
 * Created: 8/5/2017 10:03:44 AM
 * Author : QUAN1
 */ 

//INCLUDES
#include <avr/io.h>
#include <avr/interrupt.h>

//FUNCTIONS PROTOTYPES
void set_clock(void);
void led_init(void);
void tc_init(void);
void ext_s1_init(void);
void ext_s1_disable(void);
void ext_s2_init(void);
void ext_s2_disable(void);
void tc_disable(void);


//GLOBAL VARIABLE
uint8_t table[] = {0b11111110,0b11111101,0b11111011,0b11110111,0b11101111,0b11011111,0b10111111,0b01111111,0b11111111,0b10111111,0b11011111,0b11101111,0b11110111,0b11111011,0b11111101,0b11111110,0b11111111};
int i=0;
uint16_t per = 0x7A12;
uint16_t inc = 0x32DC;
int stage = 1;

//////////////////////////// MAIN ////////////////////////////////
int main(void)
{
	//initializes peripherals
	set_clock();
	led_init();
	tc_init();
	ext_s1_init();
	ext_s2_init();
	
	//enable high level interrupt
	PMIC_CTRL = PMIC_HILVLEN_bm;		//high level enable group config
	sei();
	
    while (1);
}
//////////////////////////////////////////////////////////////////


//////////////////////////// ISR /////////////////////////////////

// TCC0 OVERFLOW *************************************************
ISR(TCC0_OVF_vect)
{
	PORTC_OUT = table[i];						//output next value of table to LED
	i++;										//increment iterator i
	if(i==9 || i==17)							//if out of bounds, end game
	{
		tc_disable();
		PMIC_CTRL = 0x00;
		PORTC_OUT = 0x00;						//signals end of game
		return;
	}
	if(i == 2)									//re-enable external interrupt somewhere in the middle
	{
		PORTF_INTFLAGS = 0x02;
		ext_s2_init();
	}
	
	if(i == 10)									//re-enable external interrupt somewhere in the middle
	{
		PORTF_INTFLAGS = 0x01;
		ext_s1_init();
	}
	return;
}
// ***************************************************************


// EXTERNAL TRIGGER S1 **********************************************
ISR(PORTF_INT0_vect)
{
	if((PORTC_OUT != 0b11111110))				//if button is incorrectly pushed, disable TC and interrupts
	{
		tc_disable();
		PMIC_CTRL = 0x00;
		PORTC_OUT = 0x00;						//signals end of game
	}
	else
	{
		i=1;
		tc_disable();							//disable TC to increase speed
		if(per>0x4000)							//update period every successful round
		{
			per = per - 0x0E00;
		}
		else if(per>2000)
		{
			per = per - 0x0500;
		}
		else if(per>1000)
		{
			per = per - 0x0100;
		}
		tc_init();								//re-initialize TC with new period
		ext_s1_disable();						//disable external interrupt for switch debouncing
		
	}
}
//****************************************************************



// EXTERNAL TRIGGER S2 **********************************************
ISR(PORTF_INT1_vect)
{
	if((PORTC_OUT != 0b01111111))				//if button is incorrectly pushed, diable TC and interrupts
	{
		tc_disable();
		PMIC_CTRL = 0x00;
		PORTC_OUT = 0x00;						//signals end of game
	}
	else
	{
		i=9;
		tc_disable();							//disable TC to increase speed
		if(per>0x4000)							//update period every successful round
		{
			per = per - 0x0E00;
		}
		else if(per>2000)
		{
			per = per - 0x0500;
		}
		else if(per>1000)
		{
			per = per - 0x0100;
		}
		tc_init();								//re-initialize TC with new period
		ext_s2_disable();							//disable external interrupt for switch debouncing
		
	}
}
//****************************************************************



/////////////////////////// FUNCTIONS ////////////////////////////

// EXTERNAL INTERRUPT INIT FOR S1 ********************************
// This function initializes external interrupt for button connected to PORT F PIN 2
// Input: N/A
// Output: N/A
void ext_s1_init(void)
{
	PORTF_DIRCLR = 0x04;				//make interrupt pin as input
	PORTF_INTCTRL = 0x03;				//set high level interrupt
	PORTF_INT0MASK = 0x04;				//set pin 2 as trigger source
	PORTF_PIN2CTRL = 0x03;				//set low level trigger
}
//***************************************************************


// EXTERNAL INTERRUPT INIT FOR S2 ********************************
// This function initializes external interrupt for button connected to PORT F PIN 3
// Input: N/A
// Output: N/A
void ext_s2_init(void)
{
	PORTF_DIRCLR = 0x08;				//make interrupt pin as input
	PORTF_INTCTRL = 0x0C;				//set high level interrupt
	PORTF_INT1MASK = 0x08;				//set pin 2 as trigger source
	PORTF_PIN3CTRL = 0x03;				//set low level trigger
}
//***************************************************************



// EXTERNAL INTERRUPT DISABLE ***********************************
// This function disable external interrupt on PORT F PIN 2
// Input: N/A
// Output: N/A
void ext_s1_disable(void)
{
	PORTF_INTCTRL &= 0xFC;				//disable interrupt
	PORTF_INTFLAGS = 0x01;				//clear flag
}
// **************************************************************


// EXTERNAL INTERRUPT DISABLE ***********************************
// This function disable external interrupt on PORT F PIN 3
// Input: N/A
// Output: N/A
void ext_s2_disable(void)
{
	PORTF_INTCTRL &= 0xF3;				//disable interrupt
	PORTF_INTFLAGS = 0x02;				//clear flag
}
// **************************************************************



// TC INIT *******************************************************
// This function initializes TCC0
// Input: N/A
// Output: N/A
void tc_init(void)
{
	TCC0_PER = per;						//init period to approximately 1 second
	TCC0_INTCTRLA = 0x03;					//Overflow interrupt high level
	TCC0_CTRLA = TC_CLKSEL_DIV1024_gc;			//timer counter clock select division group config
	return;
}
//***************************************************************


// TC DISABLE ***************************************************
// This function disable TCC0
// Input: N/A
// Output: N/A
void tc_disable(void)
{
	TCC0_CTRLA = 0x00;				//timer counter clock select division group config
	TCC0_PER = 0x0000;				//init period to approximately 1 second
	TCC0_INTCTRLA = 0x00;			//Overflow interrupt high level
}
//***************************************************************



// LED INIT ******************************************************
// This function initialize PORT C directions/default settings
// Input: N/A
// Output: N/A
void led_init(void)
{
	PORTC_DIRSET = 0xFF;				//ports direction
	PORTC_OUTSET = 0xFF;				//default value
}
// ***************************************************************




// SET CLOCK *****************************************************************************************
// This function initialize system clock to 32MHz
// Input : N/A
// Output: N/A
void set_clock(void)
{
	OSC_CTRL = 0x02;						//enable 32MHz oscilaltor
	while (1)
	{
		if ((OSC_STATUS & 0x02) == 0x02)				//check if oscillator is ready
		{break;}
	}
	
	CPU_CCP = CCP_IOREG_gc;					//protect IO registers
	CLK_CTRL = 0x01;						//select 32MHz oscillator as system clock
	
	return;
}
//****************************************************************************************************



