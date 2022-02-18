#include "msp.h"
#include "csHFXT.h"
#include "csLFXT.h"
#include "HC_SF04InputCapture.h"
#include <Math.h>
#define Frequency20Hz 8192


extern volatile float distance[2];

void ConfigureTimers(void)
{



    /* Configure Timer_A1 and CCRs */
    // Set initial period in CCR0 register. This assumes timer starts at 0
    TIMER_A2->CCR[0] = Frequency20Hz;
    // Configure CCR0 for Compare mode with interrupt enabled (no output mode - 0)
    TIMER_A2->CCTL[0] = 0b0000100010010000;
    // Configure Timer_A1 in UP Mode with source ACLK prescale 1:1 and no interrupt
    TIMER_A2->CTL = 0b00100010010;  //0x0114

    /* Configure global interrupts and NVIC */
    // Enable TA1 TA1CCR0 compare interrupt
    NVIC->ISER[0] |= (1) << TA2_0_IRQn ;

    __enable_irq();

//    /* Configure Timer_A2 and CCRs */
//    // Set initial period in CCR0 register. This assumes timer starts at 0
//    TIMER_A2->CCR[0] = Frequency20Hz;
//    // Configure CCR0 for Compare mode with interrupt enabled (no output mode - 0)
//    TIMER_A2->CCTL[0] = 0x0010;
//    // Configure Timer_A2 in UP Mode with source ACLK prescale 1:1 and no interrupt
//    TIMER_A2->CTL = 0b0000000100010100;  //0x0114
//
//    /* Configure global interrupts and NVIC */
//    // Enable TA2 TA2CCR0 compare interrupt
//    NVIC->ISER[0] |= (1) << TA2_0_IRQn;
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
    ConfigureTimers();


    while(1) {}

}

// Timer A2 CCR0 interrupt service routine
//This interrupt occurs at 20 Hz to update the distance values received from the ultra sonic sensor
void TA2_0_IRQHandler(void)
{
    /* Not necessary to check which flag is set because only one IRQ
     *  mapped to this interrupt vector     */
    if (TIMER_A2->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        // TODO clear timer compare flag in TA3CCTL0
        TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //clear interrupt flag
        StartHC_SF04Reading();
        printf("\r\n Sensor 1 %4.1f (cm)  ", distance[0]);
        printf("\r\n Sensor 2 %4.1f (cm)  ", distance[1]);


    }
}
