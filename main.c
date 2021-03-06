/*!
    \file  main.c
    \brief led spark with systick, USART print and key example

    \version 2017-06-06, V1.0.0, firmware for GD32F3x0
    \version 2019-06-01, V2.0.0, firmware for GD32F3x0

    \ version 2022-05-13 heavily modified by markr1961 as part of GD32330S-start test bed.
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "main.h"
#include "hardware.h"

unsigned int SysTickCounter;
unsigned int adcStart, adcEnd;
uint16_t adcData;

uint16_t timer16PwmSetting = 500; // start at 50%.
bool bTimerSet = false;

void delay_uSec(unsigned long ulDelay_us);  // in delay_uSec.c
// functions in Timer16Pwm.c:
void InitTimer16Pwm(void);
void SetDutyCycle(uint16_t dutyCycle);

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE   (ARRAYNUM(transmitter_buffer) - 1)

uint8_t transmitter_buffer[] = "\n\rUSART interrupt test\n\r";
uint8_t receiver_buffer[UART_BUFFER_SIZE];
uint8_t transfersize = TRANSMIT_SIZE;
uint8_t receivesize = UART_BUFFER_SIZE;
__IO uint8_t txcount = 0; 
__IO uint16_t rxcount = 0; 


void Sleep(void)
{
  static int lastSystick=0;
  lastSystick = SysTickCounter;
  // sleep for up to 1 second:
  while (lastSystick == SysTickCounter)
    __WFI();
}

#define TEST_LOOPS 10000
void test_usec_delay(void)
{
    printf("waiting 1000mS.\r\n");
    delay_ms(1000);
    
    printf("starting delay_uSec() test.\r\n");
    unsigned int loop_start = SysTickCounter;
    for (int i = 0; i < TEST_LOOPS; i++)
    { 
      delay_uSec(100);
      delay_uSec(100);
      delay_uSec(100);
      delay_uSec(100);
      delay_uSec(100);
      delay_uSec(100);
      delay_uSec(100);
      delay_uSec(100);
      delay_uSec(100);
      delay_uSec(100);
//      delay_uSec(50);  // extra
    }
    unsigned int loop_end = SysTickCounter;
    printf("done\r\n");

    if ( loop_end != loop_start + TEST_LOOPS)
      printf("uS delay ERROR!\r\n");
    printf("test loops %d, delta = %d\r\n", TEST_LOOPS, loop_end - loop_start);
    printf("loop_end %d, loop_start = %d\r\n", loop_end, loop_start);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    rcu_config();
    /* configure systick */
    systick_config();
    
//    test_usec_delay();
    
    /* ADC GPIO configuration */
//    acd_gpio_config();
    /* ADC configuration */
//    adc_config();

    /* initialize the LEDs, USART and key(aka button) */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
//    gd_eval_com_init(EVAL_COM);
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);
    
    /* init Timer16 as a PWM on the default I/O */
    InitTimer16Pwm();

    /* print out the clock frequency of system, AHB, APB1 and APB2 */
//    printf("SystemCoreClock is %d\r\n", SystemCoreClock);
//    printf("CK_SYS   is %8d\r\n", rcu_clock_freq_get(CK_SYS));
//    printf("CK_AHB   is %8d\r\n", rcu_clock_freq_get(CK_AHB));
//    printf("CK_APB1  is %8d\r\n", rcu_clock_freq_get(CK_APB1));
//    printf("CK_APB2  is %8d\r\n", rcu_clock_freq_get(CK_APB2));
//    printf("CK_ADC   is %8d\r\n", rcu_clock_freq_get(CK_ADC));
//    printf("CK_USART is %8d\r\n", rcu_clock_freq_get(CK_USART));

    /* USART interrupt configuration */
    nvic_irq_enable(USART0_IRQn, 1, 0);
     
    /* initialize the UART */
    usart0_gpio_config();
    usart0_config();
    
    /* enable USART RX and TX interrupt 
        This should automatically start the TX sending and the RX receiving. 
    */  
    
    DEBUG_BREAK();

    usart_interrupt_enable(USART0, USART_INT_RBNE);
    usart_interrupt_enable(USART0, USART_INT_TBE);

    while (1)
    {
        // LED2 toggles when button is not pressed:
        if(RESET == gd_eval_key_state_get(KEY_WAKEUP))
        {
            if (!checkDelayRunning())
            {
                gd_eval_led_toggle(LED2);
                set_delay_1ms(500);
            }
        }
        else
            gd_eval_led_off(LED2);

//        // once each second, if a conversion is not in-progress, start one
//        if (((SysTickCounter % 1000) == 0)
//                && (adc_flag_get(ADC_FLAG_STRC) != SET))
//        {
//            adcStart = SysTickCounter;
//            adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
//        }
//        // once it's complete, read it:
//        if (adc_flag_get(ADC_FLAG_EOC) == SET)
//        {
//            adcEnd = SysTickCounter;
//            adcData = adc_regular_data_read();
//            //printf("conversion time %d\r\n", adcEnd - adcStart);
//            // clear the start flag so the next conversion can run
//            adc_flag_clear(ADC_FLAG_STRC);
//        }


        // update Timer16 5x per second
        if ((SysTickCounter % 200) == 0)
        {
          if (!bTimerSet)
          {
            timer16PwmSetting += 10;
            if (timer16PwmSetting >= 1000)
              timer16PwmSetting = 0;
            SetDutyCycle(timer16PwmSetting);  // set it.
            bTimerSet = true;
          }
        }
        else
          bTimerSet = false;

    if ((RESET != usart_flag_get(USART0, USART_FLAG_TC))
      && (rxcount == receivesize))
    {
      DEBUG_BREAK();
      // printf("\n\rUSART receive successfully!\n\r");
    }
 
    Sleep();

    }// wend(1)
}

/* retarget the C library printf function to the USART */
//int fputc(int ch, FILE *f)
//{
//    usart_data_transmit(EVAL_COM, (uint8_t)ch);
//    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE))
//      ;
//
//    return ch;
//}
