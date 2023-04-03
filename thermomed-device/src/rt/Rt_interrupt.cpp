#include "main.h"

#include <stm32_def.h>

/*** HAL Callback ***********************************************************/

extern "C" {

/*
void
HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    rtsys.rt_callback();
}
*/
void
HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    //
}
void
HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
    //
}

void
HAL_ADC_ErrorCallback(ADC_HandleTypeDef* hadc)
{
    //
}

void
HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    //
}

void
HAL_ADCEx_InjectedQueueOverflowCallback(ADC_HandleTypeDef* hadc)
{
    //
}

void
HAL_ADCEx_LevelOutOfWindow2Callback(ADC_HandleTypeDef* hadc)
{
    //
}

void
HAL_ADCEx_LevelOutOfWindow3Callback(ADC_HandleTypeDef* hadc)
{
    //
}

void
HAL_ADCEx_EndOfSamplingCallback(ADC_HandleTypeDef* hadc)
{
    //
}

/* extern "C" */
};

