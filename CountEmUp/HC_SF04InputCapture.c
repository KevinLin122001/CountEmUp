//File Name: HC_SF04InputCapture.c
//Author: Kevin Lin and Robert Walker
//Last Edit Date: February 19, 2022
//ECE230-03 Winter 2021-2022
//Purpose: configure Timer A0 CCR2 and CCR3 to be input capture mode
//on input pin P2.5 and P2.6
// Timer A0 CCR2 and CCR3 Capture Mode Configuration
// Timer A0 CCR2 and CCR3 CCIxA Selects external pin P2.5 and P2.6

#include <stdbool.h>
#include <msp.h>
#include <Math.h>
#include "HC_SF04InputCapture.h"

int sentTime[2];
int recievedTime[2];
float distance[2];
bool sendReceiveToggle = 0;
#define SOUNDSPEED 34300.0*pow(10,-6) //centimeters per microsecond
#define TimerAClock 1 //Mhz

void InputCaptureConfiguration(void)
{
    //trigger pin on HC-SR04 to start eight 40kHz burst
    InputCaptureTriggerPort->DIR |= InputCaptureTrigger; //output pin
    InputCaptureTriggerPort->SEL0 &= ~InputCaptureTrigger;
    InputCaptureTriggerPort->SEL1 &= ~InputCaptureTrigger;
    //Configuring P2.4 as peripheral input for capture
    InputCapturePort->DIR &= ~InputCapturePin; //input pin
    InputCapturePort->SEL0 |= InputCapturePin;  //SEL1=0 SEL0=1
    InputCapturePort->SEL1 &= ~InputCapturePin; //GPIO_PRIMARY_MODULE_FUNCTION
    //Timer A0 continuous mode at 48MHz SMCLK and 48 prescaler
    //Use SMCLK AT at 48 MHz, continuous mode, interrupt disabled
    //bits9-8=0b10 SMCLK, bits7-6=ID=0b11, bits5-4=0b10 CONTINUOUS MODE, bit1=0 NO INTERRUPT
    TIMER_A0->CTL = 0b0000001011100100;   //bits7-6=ID=0b11 x8
    //bits2-0 = TAIDEX = 0b101 = divide by 6
    //Prescaler  2^ID(TAIDEX+1)=2^3(5+1)=8X6=48 Timer clock = 48MHz/48=1MHz
    //maximum distance is 343meters(10^-6)((2^16)/2)/1MHz=11.239meters
    TIMER_A0->EX0 = 0b0000000000000101;    //bits2-0=TAIDEX=0b101 x5
    //Configuring Capture Mode: CCR1, Rising edge, CCIxA pin = P2.6, synchronous, no interrupt
    //bits15-14=01 on rising edge bits13-12=00 CCIxA bit8=1 for capture mode.
    //bit4=0 to disable interrupt. bit0=CCIFG capture interrupt flag
    TIMER_A0->CCTL[2] = 0b0100000100000000;
    TIMER_A0->CCTL[3] = 0b0100000100000000;

}

void StartHC_SF04Reading(void)
{
    //capture on rising edge first by default
    //clear capture flag, i.e., interrupt flag CCIFG
    TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCTL[3] &= ~TIMER_A_CCTLN_CCIFG;

    //first capture, wait for capture to occur when bit 0 CCIFG=1
    //Tests for the CCTL of both sensor's so that whichever returns first
    //will cause the while loop to end and continue the method.
    //This helps in identifying which sensor was triggered first
    while ((((TIMER_A0->CCTL[2]) & TIMER_A_CCTLN_CCIFG) != TIMER_A_CCTLN_CCIFG)
            || (((TIMER_A0->CCTL[3]) & TIMER_A_CCTLN_CCIFG)
                    != TIMER_A_CCTLN_CCIFG))
    {
    };
    //clear capture flag, i.e., interrupt flag CCIFG
    TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCTL[3] &= ~TIMER_A_CCTLN_CCIFG;
    //Stores the sent time
    sentTime[0] = TIMER_A0->CCR[2];
    sentTime[1] = TIMER_A0->CCR[3];

    //set to capture on falling edge by toggling bits 15 and 14
    TIMER_A0->CCTL[2] = TIMER_A0->CCTL[2] ^ 0b1100000000000000;
    TIMER_A0->CCTL[3] = TIMER_A0->CCTL[3] ^ 0b1100000000000000;

    //second capture, wait for falling edge capture to occur when bit 0 CCIFG=1
    while ((((TIMER_A0->CCTL[2]) & TIMER_A_CCTLN_CCIFG) != TIMER_A_CCTLN_CCIFG)
            || (((TIMER_A0->CCTL[3]) & TIMER_A_CCTLN_CCIFG)
                    != TIMER_A_CCTLN_CCIFG))
    {
    };

    //clear capture flag, i.e., interrupt flag CCIFG
    TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCTL[3] &= ~TIMER_A_CCTLN_CCIFG;
    //Stores the received Time
    recievedTime[0] = TIMER_A0->CCR[2];
    recievedTime[1] = TIMER_A0->CCR[3];
    //Computes the distance from the ultra sonic sensor
    distance[0] = (float) SOUNDSPEED
            * ((float) abs(recievedTime[0] - sentTime[0]) / TimerAClock) / 2.0;
    distance[1] = (float) SOUNDSPEED
            * ((float) abs(recievedTime[1] - sentTime[1]) / TimerAClock) / 2.0;
    //set to capture on rising edge by toggling bits 15 and 14
    TIMER_A0->CCTL[2] = TIMER_A0->CCTL[2] ^ 0b1100000000000000;
    TIMER_A0->CCTL[3] = TIMER_A0->CCTL[3] ^ 0b1100000000000000;

}
