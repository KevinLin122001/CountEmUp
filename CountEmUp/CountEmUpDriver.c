#include "msp.h"
#include "csHFXT.h"
#include "csLFXT.h"
#include "HC_SF04InputCapture.h"
#include "lcd8bits.h"
#include "Switch.h"
#include "Speaker.h"
#include <Math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define FrequencyHz 4096

extern volatile float distance[2];
extern bool sendReceiveToggle;
volatile float threshold = 0;
volatile float maxThreshold = 800;
volatile int currentOccupancy = 0;
volatile int maxCapacity = 3;
bool resetted = true;

//Configures Timer A2 to be UP Mode with interrupt
void ConfigureTimerA2(void)
{

    /* Configure Timer_A2 and CCRs */
    // Set initial period in CCR0 register. This assumes timer starts at 0
    TIMER_A2->CCR[0] = FrequencyHz;
    // Configure CCR0 for Compare mode with interrupt enabled (no output mode - 0)
    TIMER_A2->CCTL[0] = 0b0000100010010000;
    // Configure Timer_A2 in UP Mode with source ACLK prescale 1:1 and interrupt
    TIMER_A2->CTL = 0b00100010010;

    /* Configure global interrupts and NVIC */
    // Enable TA2 TA2CCR0 compare interrupt
    NVIC->ISER[0] |= (1) << TA2_0_IRQn;

    __enable_irq();  //Enable global interrupt
}

/**
 * Main method of the driver. Initializes subroutines and calculates an initial threshold level
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    //Initializes subroutines
    configHFXT();
    configLFXT();
    InputCaptureConfiguration();
    lcd8bits_init();
    SwitchInit();
    ConfigureTimerA2();

    //Calculates the threshold for the sonars
    int count;
    for (count = 0; count < 5; count++)
    {
        StartHC_SF04Reading();
        threshold = threshold + distance[0] + distance[1];
    }
    threshold /= 10;

    __enable_irq(); //Enable Global Interrupt

    //Ensures the program runs
    while (1)
    {

    }

}

// Timer A2 CCR0 interrupt service routine
//This interrupt occurs at 4 Hz to update the distance values received from the ultra sonic sensor
void TA2_0_IRQHandler(void)
{
    /* Not necessary to check which flag is set because only one IRQ
     *  mapped to this interrupt vector     */
    if (TIMER_A2->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
        InputCaptureTriggerPort->OUT |= InputCaptureTrigger; //trigger output pin
        int count = 0;
        for (count = 0; count < 500; count++)
            ;  //10 microsecond minimum trigger pulse
        //clear output pin to LOW
        InputCaptureTriggerPort->OUT &= ~InputCaptureTrigger; //trigger output pin

        TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //clear interrupt flag
        StartHC_SF04Reading();
        directionDetection();
    }
}
void directionDetection(void)
{
    char toPrint[256] = "";
    char toPrint2[256] = "";

    if (resetted) //Checks to see if the person has moved fully across the sensor
    {
        if (distance[0] < threshold && distance[1] >= threshold) //Checks to see if someone entered
        {
            if (currentOccupancy >= maxCapacity)  //Checks for maximum capacity
            {
                SpeakerInit(); //Initializes Speaker
                PlayNote(3500);  //Plays sharp note that irritates people
                char switchValue;
                switchValue = SwitchPort->IN & Switch; //Retrieves the switch Value
                while (switchValue == Switch ) //Waits for when the switch is pressed
                {
                    switchValue = (SwitchPort->IN & Switch );
                    int x;
                    for (x = 0; x < 144000; x++)
                        ; //Lazy debounce
                }
                InputCaptureConfiguration(); //Re-initialize input capture config since initializing speaker modified the timer setup
                PlayNote(0x1); //Plays a rest note
            }
            else //If less than maximum occupancy increment by 1
            {
                currentOccupancy += 1;
                resetted = false;
            }
        }
        else if (distance[1] < threshold && distance[0] >= threshold) //Checks to see if someone exited
        {
            if (currentOccupancy <= 0) //Prevents the current occupancy from going negative
            {
                currentOccupancy = 0;
            }
            else //Decrement by 1
            {
                currentOccupancy -= 1;
            }
            resetted = false;
        }
    }
    else //Checks to see if someone has fully passed through
    {
        if (distance[0] >= threshold && distance[0] <= maxThreshold
                && distance[1] >= threshold && distance[1] <= maxThreshold)  //Uses maxThreshold to prevent garbage random values from the sonar sensor
        {
            resetted = true; //States that the person has fully passed through
        }
    }
    lcd_clear();
    lcd_SetLineNumber(FirstLine);
    sprintf(toPrint, "        Current: %d", currentOccupancy);
    lcd_puts(toPrint);
    lcd_SetLineNumber(SecondLine);
    sprintf(toPrint2, "Max Capacity: %d", currentOccupancy);
    lcd_puts(toPrint2);
    printf("\r\n Sensor1 in %4.1f (cm)", distance[0]);
    printf("\r\n Sensor2 in %4.1f (cm)", distance[1]);
    printf("\r\n Currect: %d", currentOccupancy);
}
