/*
 * Speaker.c
 *  Subroutine file for initializing the speaker and playing a note
 *  Last Edited Date: February 19, 2022
 *  Author: Kevin Lin and Robert Walker
 */
#include <msp.h>
#include "speaker.h"

void SpeakerInit(void)
{
    SpeakerPort->DIR |= Speaker;            // set P2.4 as output
    SpeakerPort->SEL0 |= Speaker;           // P2.4 set to TA0.1
    SpeakerPort->SEL1 &= ~Speaker;  // Option 0b01

    TIMER_A0->CCTL[1] = 0x060;  //Setting TIMER_A0 to be output compare
    TIMER_A0->CTL = 0x294;
}

//Plays a note given a frequency
void PlayNote(unsigned int CurrentNote)
{
    TIMER_A0->CCR[0] = CurrentNote - 1;
    // Set high pulse-width in CCR1 register (determines duty cycle)
    TIMER_A0->CCR[1] = (CurrentNote / 2) - 1;
}
