#include "msp.h"

#define Frequency20Hz 1638

void ConfigureTimerA1(void)
{
    /* Configure Timer_A1 and CCRs */
    // Set initial period in CCR0 register. This assumes timer starts at 0
    TIMER_A1->CCR[0] = Frequency20Hz;
    // Configure CCR0 for Compare mode with interrupt enabled (no output mode - 0)
    TIMER_A1->CCTL[0] = 0x0010;
    // Configure Timer_A1 in UP Mode with source ACLK prescale 1:1 and no interrupt
    TIMER_A1->CTL = 0b0000000100010100;  //0x0114

    /* Configure global interrupts and NVIC */
    // Enable TA1 TA1CCR0 compare interrupt
    NVIC->ISER[0] |= (1) << TA1_0_IRQn;
}

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	configHFXT();
	configLFXT();
	InputCaptureConfiguration();
	ConfigureTimerA1();



}
