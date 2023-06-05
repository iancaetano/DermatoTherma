#include "RotaryEncoder.h"

#include <stm32_def.h>

#define ENC_A_Pin GPIO_PIN_6
#define ENC_A_GPIO_Port GPIOC

#define ENC_B_Pin GPIO_PIN_7
#define ENC_B_GPIO_Port GPIOC

RotaryEncoder::RotaryEncoder()
{
    //
}

void
RotaryEncoder::begin()
{
    TIM_Encoder_InitTypeDef sConfig         = {0};
    TIM_MasterConfigTypeDef sMasterConfig   = {0};
    GPIO_InitTypeDef        GPIO_InitStruct = {0};

    m_htim3.Instance                        = TIM3;
    m_htim3.Init.Prescaler                  = 0;
    m_htim3.Init.CounterMode                = TIM_COUNTERMODE_UP;
    m_htim3.Init.Period                     = 500;
    m_htim3.Init.ClockDivision              = TIM_CLOCKDIVISION_DIV1;
    m_htim3.Init.AutoReloadPreload          = TIM_AUTORELOAD_PRELOAD_DISABLE;

    sConfig.EncoderMode                     = TIM_ENCODERMODE_X1_TI1;
    sConfig.IC1Polarity                     = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection                    = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler                    = TIM_ICPSC_DIV1;
    sConfig.IC1Filter                       = 0;
    sConfig.IC2Polarity                     = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection                    = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler                    = TIM_ICPSC_DIV1;
    sConfig.IC2Filter                       = 0;

    /*** MspInit BEGIN ***/

    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin                     = ENC_A_Pin|ENC_B_Pin;
    GPIO_InitStruct.Mode                    = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull                    = GPIO_NOPULL;
    GPIO_InitStruct.Speed                   = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate               = GPIO_AF2_TIM3;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*** MspInit END ***/

    /* uses HAL_TIM_Encoder_MspInit() internally, but unused! */
    if (HAL_TIM_Encoder_Init(&m_htim3, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger       = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode           = TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(&m_htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void
RotaryEncoder::start()
{
    HAL_TIM_Encoder_Start(&m_htim3, TIM_CHANNEL_ALL);
}



void
RotaryEncoder::stop()
{
    HAL_TIM_Encoder_Stop(&m_htim3, TIM_CHANNEL_ALL);
}

int32_t
RotaryEncoder::count()
{
    this->m_counter = m_htim3.Instance->CNT;
    return m_counter;
}

int32_t
RotaryEncoder::difference()
{
    int32_t counter = count();
    int32_t diff = m_counterOld - counter;
    m_counterOld = counter;
    return -diff;
}
