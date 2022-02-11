//File Name: HC_SF04InputCapture.c
//Author: Jianjian Song
//Date: January 28, 2022
//ECE230-03 Winter 2021-2022
//Purpose: configure Timer A0 CCR1 to be input capture mode
//on input pin P2.4
// Timer A0 CCR1 Capture Mode Configuration
// Timer A0 CCR1 CCIxA Selects external pin P2.4

#include <msp.h>
#include "HC_SF04InputCapture.h"

void InputCaptureConfiguration(void) {
#define DELAYTIME 50000
    unsigned int delaycount;
    //LED1 to indicate distance
    LED1port->DIR |= LED1Pin; //output pin
    LED1port->SEL0 &=~LED1Pin;
    LED1port->SEL1 &=~LED1Pin;
    LED1port->OUT |= LED1Pin; //turn LED on
    for(delaycount=0; delaycount<DELAYTIME; delaycount++);
    LED1port->OUT &= ~LED1Pin; //turn LED off
//trigger pin on HC-SR04 to start eight 40kHz burst
    InputCaptureTriggerPort->DIR |= InputCaptureTrigger; //output pin
    InputCaptureTriggerPort->SEL0 &=~InputCaptureTrigger;
    InputCaptureTriggerPort->SEL1 &=~InputCaptureTrigger;
 //Configuring P2.4 as peripheral input for capture
    InputCapturePort->DIR &= ~InputCapturePin; //input pin
    InputCapturePort->SEL0 |= InputCapturePin;  //SEL1=0 SEL0=1
    InputCapturePort->SEL1 &=~InputCapturePin; //GPIO_PRIMARY_MODULE_FUNCTION
//Timer A0 continuous mode at 48MHz SMCLK and 48 prescaler
//Use SMCLK AT at 48 MHz, continuous mode, interrupt disabled
//bits9-8=0b10 SMCLK, bits7-6=ID=0b11, bits5-4=0b10 CONTINUOUS MODE, bit1=0 NO INTERRUPT
        TIMER_A0->CTL=0b0000001011100100;   //bits7-6=ID=0b11 x8
//bits2-0 = TAIDEX = 0b101 = divide by 6
//Prescaler  2^ID(TAIDEX+1)=2^3(5+1)=8X6=48 Timer clock = 48MHz/48=1MHz
        //maximum distance is 343meters(10^-6)((2^16)/2)/1MHz=11.239meters
        TIMER_A0->EX0 = 0b0000000000000101;    //bits2-0=TAIDEX=0b101 x5
//Configuring Capture Mode: CCR1, Rising edge, CCIxA pin = P2.6, synchronous, no interrupt
//bits15-14=01 on rising edge bits13-12=00 CCIxA bit8=1 for capture mode.
//bit4=0 to disable interrupt. bit0=CCIFG capture interrupt flag
        TIMER_A0->CCTL[1]=0b0100000100000000;
}
