/*
 * Application.c
 *
 * Created: 08/09/2022 3:50:48 AM
 *  Author: Fares
 */ 

#define F_CPU 1000000UL
#include "../ECUAL/LED/LED.h"
#include "../ECUAL/PushButton/PushButton.h"
#include "Application.h"


uint8_t normal_mode=0,ovf_counter=0,yellow_flag=0,press_flag=0,ovf_counter2=0;

//initialize the application
void app_init(){
		
		//initialize Button as input
		Button_init(PORT_D,PIN2);
		//initialize car traffic lights' as output
		LED_init(PORT_A,GREEN);
		LED_init(PORT_A,YELLOW);
		LED_init(PORT_A,RED);
		//initialize pedestrian traffic lights' as output
		LED_init(PORT_B,GREEN);
		LED_init(PORT_B,YELLOW);
		LED_init(PORT_B,RED);
		//Normal mode Timer 0
		CLEAR_BIT(TCCR0,WGM00);
		CLEAR_BIT(TCCR0,WGM01);
		//Normal mode Timer 2
		CLEAR_BIT(TCCR2,WGM20);
		CLEAR_BIT(TCCR2,WGM21);
		//initial value Timer 0
		TCNT0=INITIAL_TIMER_0 ;
		//initial value Timer 2
		TCNT2=INITIAL_TIMER_2;
		//Enable OVF Interrupt Timer 0
		TIMSK|=(1<<TOIE0);
		//Enable OVF Interrupt Timer 2
		TIMSK|=(1<<TOIE2);
		//Enable External Interrupt Request 0
		GICR |=(1<<INT0);
		// Any logical change
		MCUCR |=(1<<ISC00);
		MCUCR &=~(1<<ISC01);
	}

//Run the application
void app_start()
{
	while(1)
{

//Prescaler 1024 - Start timer0
SET_BIT(TCCR0,CS00);
CLEAR_BIT(TCCR0,CS01);
SET_BIT(TCCR0,CS02);
//Enable GIE
SREG|=(1<<GIE);

//Start Normal mode
	switch (normal_mode)
	{
	case CAR_GREEN  :
	case CAR__GREEN :
	/* car's traffic light: GREEN
	*/
	LED_off(PORT_A,RED);
	LED_on(PORT_A,GREEN);
	LED_off(PORT_A,YELLOW);

	break;
	
	case CAR__YELLOW  :
	case CAR_YELLOW   :
	/* car's traffic light: YELLOW
	*/
	yellow_flag=1;
	LED_off(PORT_A,RED);
	LED_off(PORT_A,GREEN);
	


	
	break;
	
	case CAR_RED  :
	 /*
	car's traffic light: RED
	*/		
	LED_off(PORT_A,YELLOW);
	LED_off(PORT_A,GREEN);
	LED_on(PORT_A,RED);

	break;
	}
	
	/*
	 Car's traffic light       :  Green->Yellow->Red->Yellow->Green->Repeat
	 */
if (normal_mode>TRAFFIC_STATE_REPEAT)
{
	normal_mode=0;
}
	
}
}

void emergency_red()
{
	
	SREG&=~(1<<GIE);
	// Stop timer0

	while(ovf_counter<OVERFLOWS)
	{
		/* car's traffic light: Red
		   pedestrian's traffic light: Green
	     */
		LED_on(PORT_A,RED);
		LED_off(PORT_A,GREEN);
		LED_off(PORT_A,YELLOW);
		LED_off(PORT_B,RED);
		LED_on(PORT_B,GREEN);
		LED_off(PORT_B,YELLOW);
		// Check Timer flag
		while(((TIFR) & (1<<TOV0))==0);
        //clear flag
		TIFR |=(1<<TOV0);
		ovf_counter++;
	}
	ovf_counter=0;
	
	
	// Stop timer0
	CLEAR_BIT(TCCR0,CS00);
	CLEAR_BIT(TCCR0,CS01);
	CLEAR_BIT(TCCR0,CS02);
	//pedestrian's traffic light off
	LED_off(PORT_B,RED);
	LED_off(PORT_B,GREEN);
	LED_off(PORT_B,YELLOW);
	// return to yellow traffic light state
	normal_mode=CAR_YELLOW;
	// Set Global Interrupt Enable
	SREG|=(1<<GIE);
}

void emergency_green()
{
	
	SREG&=~(1<<GIE);
	
	// Stop timer0
	CLEAR_BIT(TCCR0,CS00);
	CLEAR_BIT(TCCR0,CS01);
	CLEAR_BIT(TCCR0,CS02);
	
	//Prescaler 1024 - Start timer0
	SET_BIT(TCCR0,CS00);
	CLEAR_BIT(TCCR0,CS01);
	SET_BIT(TCCR0,CS02);
	
	ovf_counter=0;
	while(ovf_counter<OVERFLOWS)
	{
		/* car's traffic light: Yellow
		   pedestrian's traffic light: Yellow
	     */
		if(ovf_counter%2==0){
		LED_off(PORT_A,RED);
		LED_off(PORT_A,GREEN);
		LED_toggle(PORT_A,YELLOW);
		LED_off(PORT_B,RED);
		LED_off(PORT_B,GREEN);
		LED_toggle(PORT_B,YELLOW);
		}
		while(((TIFR) & (1<<TOV0))==0);
		//clear flag
		TIFR |=(1<<TOV0);
		ovf_counter++;
	}
	ovf_counter=0;
	
	while(ovf_counter<OVERFLOWS)
	{
		 /* car's traffic light: Red
		   pedestrian's traffic light: Green
	     */
		LED_on(PORT_A,RED);
		LED_off(PORT_A,GREEN);
		LED_off(PORT_A,YELLOW);
		LED_off(PORT_B,RED);
		LED_on(PORT_B,GREEN);
		LED_off(PORT_B,YELLOW);
		
		while(((TIFR) & (1<<TOV0))==0);
		//clear flag
		TIFR |=(1<<TOV0);
		ovf_counter++;
	}

	
	// Stop timer0
	CLEAR_BIT(TCCR0,CS00);
	CLEAR_BIT(TCCR0,CS01);
	CLEAR_BIT(TCCR0,CS02);
	
//pedestrian's traffic light off
  LED_off(PORT_B,RED);
  LED_off(PORT_B,GREEN);
  LED_off(PORT_B,YELLOW);
  // return to yellow traffic light state
  normal_mode=CAR_YELLOW;
	// Set Global Interrupt Enable
	SREG|=(1<<GIE);
}
//Interrupt to enter pedestrian mode
ISR(EXT_INT_0)
{
	
	press_flag++;
	switch(press_flag){
		case 1 :
		//Prescaler 1024 - Start timer2
		SET_BIT(TCCR2,CS20);
		SET_BIT(TCCR2,CS21);
		SET_BIT(TCCR2,CS22);
		break;
		case 2:
		if (ovf_counter2>SHORT_PRESS)
		{
            // This is a long press
		}
		else
		{
			// This is a short press
			switch (normal_mode)
			{
				
				case CAR__GREEN  :
				case CAR_GREEN  :
				case CAR_YELLOW  :
				case CAR__YELLOW  :

				emergency_green();
				
				break;

				case CAR_RED  :
                    emergency_red();
				break;

			}
		}
		
		//Stop Timer and reset press checking's variables
		CLEAR_BIT(TCCR2,CS20);
		CLEAR_BIT(TCCR2,CS21);
		CLEAR_BIT(TCCR2,CS22);
		ovf_counter2=0;
		press_flag=0;
		break;
	}
	
		

	
}

/*
TIMER
------
1-switch from state to another in specific time
2- toggle yellow led in a specific pattern
*/
ISR(TIMER_0_OVF)
{
	//OVF flag Cleared automatically by hardware
	if (ovf_counter<OVERFLOWS)
	{
		
		TCNT0=12;
		ovf_counter++;
		//Toggle both yellow traffic light's 
		if(yellow_flag==1 && (ovf_counter% 2==0) )
		{
		  LED_toggle(PORT_A,YELLOW);
		  
			
		}
		else{}
		
		
	} 

	
	else
	{
		//reset variable and enter the next traffic light's state
		yellow_flag=0;
		normal_mode++;
		ovf_counter=0;
	}
  	
}

// Timer to check if the button short or long pressed
ISR(TIMER_2_OVF)
{
	//incase the button is broken or an issue that causes the button to be pressed for a long time
		if(ovf_counter2==255)
		{
			
			//Stop Timer and reset press checking's variables
			CLEAR_BIT(TCCR2,CS20);
			CLEAR_BIT(TCCR2,CS21);
			CLEAR_BIT(TCCR2,CS22);
		}
		else
		// in normal cases
		{
			
		
		TCNT2=12;
		
		ovf_counter2++;
		}
	
}













