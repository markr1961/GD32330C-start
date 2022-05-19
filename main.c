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

#include "gd32f3x0.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32330c-start.h"

unsigned int SysTickCounter;
unsigned int adcStart, adcEnd;
uint16_t adcData;

void delay_uSec(unsigned long ulDelay_us);  // in delay_uSec.c

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);
    /* ADC regular channel config */
    adc_regular_channel_config(0U, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);

    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

    ///* ADC discontinuous mode */
    //adc_discontinuous_mode_config(ADC_REGULAR_CHANNEL, 1U);
    adc_special_function_config(ADC_SCAN_MODE, DISABLE);

    /* enable ADC interface */
    adc_enable();
    delay_uSec(1000);

    /* ADC calibration and reset calibration */
    adc_calibration_enable();

}
/*!
    \brief      toggle the led every 500ms
    \param[in]  none
    \param[out] none
    \retval     none
*/
void led_spark(void)
{
    static __IO uint32_t timingdelaylocal = 0U;

    if(timingdelaylocal)
    {

        if(timingdelaylocal < 500U)
        {
            gd_eval_led_on(LED1);
        }
        else
        {
            gd_eval_led_off(LED1);
        }

        timingdelaylocal--;
    }
    else
    {
        timingdelaylocal = 1000U;
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

#define TEST_LOOPS 10000
int main(void)
{
    rcu_config();
    /* configure systick */
    systick_config();
    
    unsigned int loop_start = SysTickCounter;
    for (int i = 0; i < TEST_LOOPS; i++)
          delay_uSec(1000);
    unsigned int loop_end = SysTickCounter;

    if ( loop_end > loop_start + TEST_LOOPS + 1)
      printf("uS delay ERROR! \r\n");
    printf("test loops %d, delta = %d\r\n", TEST_LOOPS, loop_end - loop_start);
    printf("loop_end %d, loop_start = %d\r\n", loop_end, loop_start);
    
    /* ADC GPIO configuration */
    gpio_config();
    /* ADC configuration */
    adc_config();

    /* initilize the LEDs, USART and key */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_com_init(EVAL_COM);
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_GPIO);

    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    printf("SystemCoreClock is %d\r\n", SystemCoreClock);
    printf("CK_SYS   is %8d\r\n", rcu_clock_freq_get(CK_SYS));
    printf("CK_AHB   is %8d\r\n", rcu_clock_freq_get(CK_AHB));
    printf("CK_APB1  is %8d\r\n", rcu_clock_freq_get(CK_APB1));
    printf("CK_APB2  is %8d\r\n", rcu_clock_freq_get(CK_APB2));
    printf("CK_ADC   is %8d\r\n", rcu_clock_freq_get(CK_ADC));
    printf("CK_USART is %8d\r\n", rcu_clock_freq_get(CK_USART));

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

        // once each second, if a conversion is not in-progress, start one
        if (((SysTickCounter % 1000) == 0)
                && (adc_flag_get(ADC_FLAG_STRC) != SET))
        {
            adcStart = SysTickCounter;
            adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
        }
        // once it's complete, read it:
        if (adc_flag_get(ADC_FLAG_EOC) == SET)
        {
            adcEnd = SysTickCounter;
            adcData = adc_regular_data_read();
            //printf("conversion time %d\r\n", adcEnd - adcStart);
            // clear the start flag so the next conversion can run
            adc_flag_clear(ADC_FLAG_STRC);
        }
    }
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
