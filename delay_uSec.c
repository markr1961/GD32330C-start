//
// modified from https://community.nxp.com/t5/Kinetis-Microcontrollers/How-to-generate-microsecond-delay-with-SysTick-on-KL25/ta-p/1104019
//
#include <stdint.h>
#include "gd32f3x0.h"

// This delay ensures at least the requested delay in uSec
// - it may be extended by interrupts occurring during its uSec wait loop that take longer that 1us to complete
// - it's main use is for short delays to ensure respecting minimum hardware stabilization times and such

#define CORE_US (SystemCoreClock/1000000) // the number of core clocks in a uSec
void delay_uSec(unsigned long ulDelay_us)
{
    // ensure that the compiler puts the variable in a register rather than work with it on the stack
//  register unsigned long ulPresentSystick;
    register unsigned long ulMatch;
    register unsigned long _ulDelay_us = ulDelay_us;
    if (_ulDelay_us == 0)   // minimum delay is 1us
    {
        _ulDelay_us = 1;
    }
    SysTick->CTRL |= SysTick_CTRL_COUNTFLAG_Msk;  // clear the SysTick reload flag

    ulMatch = ((SysTick->VAL - CORE_US) & SysTick_VAL_CURRENT_Msk);   // first 1uSec match value (SysTick counts down)
    do
    {
        // wait until the uSec period has expired
        while (SysTick->VAL > ulMatch)
        {
            // if we missed a reload (that is, the SysTick was reloaded with its reload value after reaching zero)                (void)SYSTICK_CSR;
            // clear the SysTick reload flag
            if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0)
            {
                break;  // assume a single uSec period expired
            }
        }
        ulMatch -= CORE_US;             // set the next 1uSec match
        ulMatch &= SysTick_VAL_CURRENT_Msk;  // respect SysTick 24 bit counter mask
    }
    while (--_ulDelay_us != 0);  // one uSec period has expired so count down the requested periods until zero
}
