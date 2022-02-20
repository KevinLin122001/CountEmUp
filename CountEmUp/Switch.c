/*
 * Switch.c
 * Subroutine file for initializing the switch
 * Last Edited Date: February 19, 2022
 * Author: Kevin Lin and Robert Walker
 */

#include <msp.h>
#include "Switch.h"

void SwitchInit(void)
{
    //Initialize as general I/O, SEL0 = 0 and SEL1 = 0
    SwitchPort->SEL0 &= ~(Switch);
    SwitchPort->SEL1 &= ~(Switch);
    //Sets Direction to be Input (bit = 0)
    SwitchPort->DIR &= ~(Switch);
    //Sets the pullup output bit to be 1
    SwitchPort->OUT |= (Switch);
    //Enables pull resistor
    SwitchPort->REN |= (Switch);

    NVIC->ISER[1] = 1 << (PORT3_IRQn & 5);
}
