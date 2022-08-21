/*
 * Application.h
 *
 * Created: 08/09/2022 3:51:02 AM
 *  Author: Fares
 */ 


#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "../ECUAL/LED/LED.h" 
#include "../ECUAL/PushButton/PushButton.h" 
/************************************************************************/
/*                           MACRO                                      */
/************************************************************************/

#define RED                   PIN2
#define YELLOW                PIN1
#define GREEN                 PIN0
#define CAR_GREEN             0
#define CAR__GREEN            4
#define CAR__YELLOW           1
#define CAR_YELLOW            3
#define CAR_RED               2
#define OVERFLOWS             20
#define SHORT_PRESS           4
#define TRAFFIC_STATE_REPEAT  4
#define INITIAL_TIMER_0       12
#define INITIAL_TIMER_2       12

/************************************************************************/
/*                              API                                     */
/************************************************************************/

void app_init(void);
void app_start(void);
void emergency_red(void);
void emergency_green(void);

/************************************************************************/
/*                           Calculations                               */
/************************************************************************/

/*

T_tick=1024/1000000=1.024ms
T_maxdelay=1.024ms*2^8=262.144ms

Timer 0
-------
N_overflows=(Ceil)5000/262.144 approx=20
Timer_initial_value=2^8 - (5000/1.024)/20=11.86 approx=12

Timer 2
-------
N_overflows=(Ceil)1000/262.144 approx=4
Timer_initial_value=2^8 - (1000/1.024)/4=11.86 approx=12
SHORT PRESS = 1 SEC = 12 overflows

*/
#endif /* APPLICATION_H_ */