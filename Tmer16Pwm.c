
#include "gd32f3x0.h"
#include <stdio.h>

//
// forward declarations
void SetDutyCycle(uint16_t dutyCycle);
void PWM_On(void);

// Timer clock constants
//
// Both APB buses are running at 2MHz (the timers run at APB*2). 
// A 4KHz PWM output with 1000 counts requires no prescaler.
//#define PRESCALE_DIVIDER  10000000    // Originally 1000000 */
#define CLOCK_DIVIDER     1000        // Clock divider for PWM at 4KHz frequency (0.250uS/tick)

////////////////////////////////////////////////////////////////////////////////
///
//  @brief    Init Timer16 as a PWM output on GPIO PB9
//  @param    none
//  @return   none
////////////////////////////////////////////////////////////////////////////////
void InitTimer16Pwm(void)
{
  timer_parameter_struct    TIM_TimeBaseStructure;
  timer_oc_parameter_struct TIM_OCInitStructure;

  // Turn on peripherals we need, GPIOB
  rcu_periph_clock_enable(RCU_GPIOB);
  // This shouldn't be necessary:
  // enable the clock to the SysCfg block so the AF source can be set can be set:
  // rcu_periph_clock_enable(RCU_CFGCMP);
  
  gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
  gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

  // use Pin9 alternate function remapping to TIMER16
  gpio_af_set(GPIOB, GPIO_AF_2, GPIO_PIN_9);

  // Enable Timer16 clock and reset to to defaults (also releases reset)
  rcu_periph_clock_enable(RCU_TIMER16);
  timer_deinit(TIMER16);
  // TIM 15/16/17 do not have conifgurable clock sources
  //timer_internal_clock_config(TIMER16);

  // Timer Base configuration
  timer_struct_para_init(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.prescaler = 21-1; // 42MHz/22= 2MHz input
  TIM_TimeBaseStructure.counterdirection = TIMER_COUNTER_UP;
  TIM_TimeBaseStructure.period = CLOCK_DIVIDER;
  TIM_TimeBaseStructure.clockdivision = TIMER_CKDIV_DIV1;
  TIM_TimeBaseStructure.repetitioncounter = 0;

  timer_init(TIMER16, &TIM_TimeBaseStructure);

  // CH0 Configuration in PWM mode.  We're using TIMER16 CH0,
  // which has been mapped to output the PWM pulse on PB9.
  timer_channel_output_struct_para_init(&TIM_OCInitStructure);

  TIM_OCInitStructure.outputstate  = TIMER_CCX_ENABLE;
  TIM_OCInitStructure.outputnstate = TIMER_CCXN_DISABLE;
  TIM_OCInitStructure.ocpolarity   = TIMER_OC_POLARITY_HIGH;
//  TIM_OCInitStructure.ocnpolarity  = TIMER_OCN_POLARITY_LOW;
  TIM_OCInitStructure.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
//  TIM_OCInitStructure.ocnidlestate = TIMER_OCN_IDLE_STATE_HIGH;

  timer_channel_output_config(TIMER16, TIMER_CH_0, &TIM_OCInitStructure);

  timer_channel_output_mode_config(TIMER16, TIMER_CH_0, TIMER_OC_MODE_PWM0);
  
  timer_primary_output_config(TIMER16, ENABLE);

  timer_counter_value_config(TIMER16, 0); // Sometimes, there's trash in this reg!

  // Double buffered
  timer_channel_output_shadow_config(TIMER16, TIMER_CH_0, TIMER_OC_SHADOW_ENABLE);
  /* auto-reload preload enable */
  timer_auto_reload_shadow_enable(TIMER16);
  
  SetDutyCycle(500);  // set initial to 50%
  PWM_On();
}

////////////////////////////////////////////////////////////////////////////////
///
//  @brief      Set timer16 duty cycle
//  @param      dutyCycle
//  @return     none
////////////////////////////////////////////////////////////////////////////////
void SetDutyCycle(uint16_t dutyCycle)
{
  timer_channel_output_pulse_value_config(TIMER16, TIMER_CH_0, dutyCycle);

}

////////////////////////////////////////////////////////////////////////////////
///
//  @brief    turn on Timer16 PWM
//  @param    none
//  @return   none
////////////////////////////////////////////////////////////////////////////////
void PWM_On(void)
{
  timer_enable(TIMER16);

  // Turn on CH0 output on TIM16
  timer_channel_output_state_config(TIMER16, TIMER_CH_0, TIMER_CCX_ENABLE);

}
