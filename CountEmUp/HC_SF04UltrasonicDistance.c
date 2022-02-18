//Jianjian Song
//ECE230-03 Winter 2021-2022
//February 4, 2022
//measure pulse period of an input signal with input capture mode
//Set Timer A to run in continuous mode and Timer A CCR1 in capture mode
//
/*******************************************************************************
* MSP432 Timer_A - VLO Period Capture
*
* Description: Capture one rising edge and one falling edge to find pulse width and store them in
* an array. Calculate the period of the signal from the two captured counts.
*
*                MSP432P4111
*             ----------------------
*         /|\|                      |
*          | |                      |
*          --|RST         P1.0      |---> P1.0 LED, sampling and distance indicator
*            |            P2.4      |--- TA0.1 SRF04 Echo Pulse, CCR1 input capture
*            |            P1.7      |---> Trigger pulse on SRF04
*            |                      |
*******************************************************************************/
/* DriverLib Includes */
#include <msp.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "HC_SF04InputCapture.h"
#include "csHFXT.h"
#include "csLFXT.h"


float getEchoPulse(void);
#define THRESHOLD 30 //centimeters

#define SOUNDSPEED 34300.0*pow(10,-6) //centimeters per microsecond
int main(void)
{
    float ObjectDistance;
    unsigned int count;
    float PulseWidth;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    //Use External 48MHz oscillator
    //Set MCLK at 48 MHz for CPU
    //Set SMCLK at 48 MHz
    //Set ACLK at 128kHz
    configHFXT();
    configLFXT();
    InputCaptureConfiguration();

    while(1){
    //send negative or falling trigger pulse to start measurement.
    //Set output pin HIGH
        InputCaptureTriggerPort->OUT |= InputCaptureTrigger; //trigger output pin
        for (count=0; count<500;count++);  //10 microsecond minimum trigger pulse
   //clear output pin to LOW
        InputCaptureTriggerPort->OUT &= ~InputCaptureTrigger; //trigger output pin

        PulseWidth = getEchoPulse();
        ObjectDistance = (float) SOUNDSPEED*PulseWidth/2.0;
        printf("\r\n object distance in %4.1f (cm)  pulse width %4.1f (us)", ObjectDistance, PulseWidth);
        if(ObjectDistance<THRESHOLD)   LED1port->OUT |= LED1Pin; //turn on LED1
        else   LED1port->OUT &= ~LED1Pin;
        //Delay times allow for approximately 2 measurements of distance for when someone walks by it
        for(count=0; count<500000;count++);
    }; //end while(1)
} //end main()


#define TimerAClock 1    //MHz
//maximum distance is 343meters(10^-6)((2^16)/2)/1MHz=11.239meters

//Input capture on rising edge and then falling edge
//to find count difference corresponding to echo time
//Timer A0 pointer is used but corresponding DriveLib functions are shown
float getEchoPulse(void)
{
#define NUMBER_TIMER_CAPTURES 2
static uint_fast16_t timerAcaptureValues[NUMBER_TIMER_CAPTURES];
float period_measured;
//toggle LED1 to show input capture is active
    LED1port->OUT ^= LED1Pin; //toggle LED1
//capture on rising edge first by default
//clear capture flag, i.e., interrupt flag CCIFG
    TIMER_A0->CCTL[1]&=~TIMER_A_CCTLN_CCIFG;
//    TIMER_A0->CCTL[1]=(TIMER_A0->CCTL[1])&0xFFFE;

//first capture, wait for capture to occur when bit 0 CCIFG=1
    while(((TIMER_A0->CCTL[1])&TIMER_A_CCTLN_CCIFG)!=TIMER_A_CCTLN_CCIFG) {};
//clear capture flag, i.e., interrupt flag CCIFG
    TIMER_A0->CCTL[1]&=~TIMER_A_CCTLN_CCIFG;
//get first count value
    timerAcaptureValues[0] = TIMER_A0->CCR[1];

//set to capture on falling edge by toggling bits 15 and 14
   TIMER_A0->CCTL[1]=TIMER_A0->CCTL[1]^0b1100000000000000;
//second capture, wait for falling edge capture to occur when bit 0 CCIFG=1
   while(((TIMER_A0->CCTL[1])&TIMER_A_CCTLN_CCIFG)!=TIMER_A_CCTLN_CCIFG) {};
//clear capture flag, i.e., interrupt flag CCIFG
   TIMER_A0->CCTL[1]&=~TIMER_A_CCTLN_CCIFG;
//get second count value
   timerAcaptureValues[1] = TIMER_A0->CCR[1];
   period_measured = (float) abs(timerAcaptureValues[0]-timerAcaptureValues[1])/TimerAClock;
// return to rising edge by toggling bits 15 and 14
   TIMER_A0->CCTL[1]=TIMER_A0->CCTL[1]^0b1100000000000000;
   //toggle LED1 to show input capture is over
       LED1port->OUT ^= LED1Pin; //toggle LED1
   return period_measured;
}


