/*
 * hal_adc.c
 *
 *  Created on: Mar 1, 2019
 *      Author: sglas
 */

#include <msp430.h>
#include <stdint.h>

#include "adc.h"
#include "project_settings.h"

void hal_ADC_Init(void) {
    ADC12CTL0 = ADC12SHT02 + ADC12ON + ADC12REFON + ADC12REF2_5V;         // Sampling time, ADC12 on, Reference generator on, 2.5V reference
    ADC12CTL1 = ADC12SHP;                     // Use sampling timer
    ADC12IE = 0x01;                           // Enable interrupt
    ADC12CTL0 |= ADC12ENC;
}

void hal_ADC_StartChannel(uint8_t channel) {
    // Channel 1 is P6.0
    ADC12CTL0 |= ADC12SC;                    // Start sampling/conversion

    P6SEL |= channel;                        // channel [P6.0] ADC option select
    ADC12CTL0 |= ADC12SC | ADC12ENC;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0
      ADC_ProcessMeasurementFromISR(ADC12MEM0);
      break;
  case  8:
      //ADC_ProcessMeasurementFromISR(ADC12MEM0);
      break;                                // Vector  8:  ADC12IFG1
  case 10:
      //ADC_ProcessMeasurementFromISR(ADC12MEM0);
      break;                           // Vector 10:  ADC12IFG2
  case 12:
      //ADC_ProcessMeasurementFromISR(ADC12MEM0);
      break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}
#endif
