#include "rt/Rf_hardware.h"

#include <Arduino.h>



#define RF_ADC_FEEDBACK_CURRENT     GPIO_PIN_3
#define RF_ADC_FEEDBACK_GPIO_Port   GPIOA

#define RF_ADC_FEEDBACK_PHASE       GPIO_PIN_4
#define RF_ADC_FEEDBACK_GPIO_Port   GPIOA

#define RF_ADC_FEEDBACK_VDC         GPIO_PIN_6
#define RF_ADC_FEEDBACK_GPIO_Port   GPIOA

#define RF_ENABLE_Pin               GPIO_PIN_4
#define RF_ENABLE_GPIO_Port         GPIOC

#define RF_DEBUG_Pin                GPIO_PIN_2
#define RF_DEBUG_GPIO_Port          GPIOA

#define ADC_12BIT                   4096.0f
#define ADC_VCC                     3.3f

const float Rf_hardware::GAIN_CONTROL_MIN = 0.0f;
const float Rf_hardware::GAIN_CONTROL_MAX = 3.0f;

static TIM_HandleTypeDef            htim2;

Rf_hardware::Rf_hardware()
{
    //
}

void
Rf_hardware::beginTimer2()
{
    TIM_ClockConfigTypeDef  sClockSourceConfig  = {0};
    TIM_MasterConfigTypeDef sMasterConfig       = {0};

    /* 170 MHz / 1700 = 100 kHz */
    /* 100 kHz / 200 = 500 Hz = 2 ms */
    htim2.Instance                              = TIM2;
    htim2.Init.Prescaler                        = 1700;
    htim2.Init.CounterMode                      = TIM_COUNTERMODE_UP;
    htim2.Init.Period                           = 200;
    htim2.Init.ClockDivision                    = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload                = TIM_AUTORELOAD_PRELOAD_DISABLE;

    /*** MspInit BEGIN ***/

    __HAL_RCC_TIM2_CLK_ENABLE();

    /*** MspInit END ***/

    /* uses HAL_TIM_Base_MspInit() internally, but unused! */
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger           = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode               = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
}



void
Rf_hardware::beginGpio()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, RF_ENABLE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RF_ENABLE_GPIO_Port, RF_DEBUG_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : RF_ENABLE_Pin */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RF_ENABLE_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : RF_DEBUG_Pin */
    GPIO_InitStruct.Pin = RF_DEBUG_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = 0; // macroname?
    HAL_GPIO_Init(RF_DEBUG_GPIO_Port, &GPIO_InitStruct);
}

void
Rf_hardware::begin()
{
    beginTimer2();
    beginGpio();

    
    /* Start TIM2 and ADC1 */
	if (HAL_TIM_Base_Start(&htim2) != HAL_OK) {
        Error_Handler();
    }

}


/*** HW interface ***********************************************************/
void
Rf_hardware::pke_enable()
{
    HAL_GPIO_WritePin(RF_ENABLE_GPIO_Port, RF_ENABLE_Pin, GPIO_PIN_RESET);
}

void
Rf_hardware::pke_disable()
{
    HAL_GPIO_WritePin(RF_ENABLE_GPIO_Port, RF_ENABLE_Pin, GPIO_PIN_SET);
}




void
Rf_hardware::set_debug_pin_state(bool state)
{
    if (state) {
        HAL_GPIO_WritePin(RF_DEBUG_GPIO_Port, RF_DEBUG_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(RF_DEBUG_GPIO_Port, RF_DEBUG_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief Read ADC voltage, proportional to current
 * 
 * @return float ADC voltage
 */
float
Rf_hardware::read_adc_voltage()
{
    //// READ I FEEDBACK HERE IAN
}

/**
 * @brief Set DAC voltage. Must be between 1.0V and 2.9V.
 * 
 * @param v value between 1.0 and 2.9
 */
void
Rf_hardware::set_dac_voltage(float v)
{
    if (v < GAIN_CONTROL_MIN) {
        v = GAIN_CONTROL_MIN;
    } else if (v > GAIN_CONTROL_MAX) {
        v = GAIN_CONTROL_MAX;
    }
        //// Set VOLTAGE HERE IAN
    uint32_t value = static_cast<uint32_t>(ADC_12BIT * v / ADC_VCC);

}

/// WRITE NEW FUNCTION TO READ PHASE HERE IAN
/// WRITE NEW FUNCTION TO READ VDC HERE IAN