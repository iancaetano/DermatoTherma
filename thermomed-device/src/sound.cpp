#include <Arduino.h>
#include "sound.h"
#include "rt/Rt_system.h"
#include "TreatmentTimeHandler.h"
#include <chrono> // For time tracking
#include <sys/time.h>

#define AUDIO_ENABLE_Pin                   GPIO_PIN_5
#define AUDIO_ENABLE_GPIO_Port             GPIOC

#define AUDIO_DAC_Pin                      GPIO_PIN_6
#define AUDIO_DAC_GPIO_Port                GPIOA

DAC_HandleTypeDef hdac2;
DMA_HandleTypeDef hdma_dac2;

TIM_HandleTypeDef htim7;



#define NS  128
uint32_t Wave_LUT[NS] = {
    2048, 2149, 2250, 2350, 2450, 2549, 2646, 2742, 2837, 2929, 3020, 3108, 3193, 3275, 3355,
    3431, 3504, 3574, 3639, 3701, 3759, 3812, 3861, 3906, 3946, 3982, 4013, 4039, 4060, 4076,
    4087, 4094, 4095, 4091, 4082, 4069, 4050, 4026, 3998, 3965, 3927, 3884, 3837, 3786, 3730,
    3671, 3607, 3539, 3468, 3394, 3316, 3235, 3151, 3064, 2975, 2883, 2790, 2695, 2598, 2500,
    2400, 2300, 2199, 2098, 1997, 1896, 1795, 1695, 1595, 1497, 1400, 1305, 1212, 1120, 1031,
    944, 860, 779, 701, 627, 556, 488, 424, 365, 309, 258, 211, 168, 130, 97,
    69, 45, 26, 13, 4, 0, 1, 8, 19, 35, 56, 82, 113, 149, 189,
    234, 283, 336, 394, 456, 521, 591, 664, 740, 820, 902, 987, 1075, 1166, 1258,
    1353, 1449, 1546, 1645, 1745, 1845, 1946, 2047
};
uint32_t *ptrLUT = Wave_LUT;




void SoundSender::begin(void)
{
  MX_GPIO_Init();
  //MX_DMA_Init();
  MX_DAC2_Init();
  MX_TIM7_Init();

  if (HAL_TIM_Base_Start(&htim7) != HAL_OK) {
        Error_Handler();
    }

  if(HAL_DAC_Start_DMA(&hdac2, DAC_CHANNEL_1, (uint32_t*)Wave_LUT, NS, DAC_ALIGN_12B_R)!=HAL_OK){
    Error_Handler();
  }

  sound_disable();
  changeVolume(0.1);

}

void 
SoundSender::MX_GPIO_Init(void)
{
     __HAL_RCC_GPIOA_CLK_ENABLE();
     __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, AUDIO_ENABLE_Pin, GPIO_PIN_RESET);


    /*Configure GPIO pins : RF_ENABLE_Pin */
    GPIO_InitStruct.Pin = AUDIO_ENABLE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(AUDIO_ENABLE_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin                         = AUDIO_DAC_Pin;
    GPIO_InitStruct.Mode                        = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull                        = GPIO_NOPULL;
    HAL_GPIO_Init(AUDIO_DAC_GPIO_Port, &GPIO_InitStruct);

}

void SoundSender::MX_DAC2_Init(void)
{
  __HAL_RCC_DAC2_CLK_ENABLE();

  DAC_ChannelConfTypeDef sConfig = {0};

  hdac2.Instance = DAC2;
  if (HAL_DAC_Init(&hdac2) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_T7_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac2, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  

  //__HAL_RCC_DMA1_CLK_ENABLE();

  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

 
  hdma_dac2.Instance = DMA1_Channel2;
  hdma_dac2.Init.Request = DMA_REQUEST_DAC2_CHANNEL1;
  hdma_dac2.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_dac2.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_dac2.Init.MemInc = DMA_MINC_ENABLE;
  hdma_dac2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_dac2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_dac2.Init.Mode = DMA_CIRCULAR;
  hdma_dac2.Init.Priority = DMA_PRIORITY_MEDIUM;
 
  __HAL_LINKDMA(&hdac2, DMA_Handle1, hdma_dac2);

  if (HAL_DMA_Init(&hdma_dac2) != HAL_OK)
    {
      Error_Handler();
    }


}

 
 void SoundSender::MX_TIM7_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 0;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 800;
  htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  __HAL_RCC_TIM7_CLK_ENABLE();

  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim7, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/**
  * Enable DMA controller clock
  */
void SoundSender::MX_DMA_Init(void)
{

  /* DMA controller clock enable */


}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */





void SoundSender::TreatmentTone(float temp)
{
    static auto start_time = std::chrono::high_resolution_clock::now(); // Start time

    if(!treatmentTimeHandler.Treshexeeded()){
        int toneAt50 = 1300;
        int toneAt20 = 1000;
        int dt = toneAt50-toneAt20;
        int dT = 30;
        uint32_t timerPeriod = toneAt50 - dt/dT*temp;
        setPeriod(&htim7,timerPeriod);
    } else if(treatmentTimeHandler.Treshexeeded()){
        // Get current time
        auto current_time = std::chrono::high_resolution_clock::now();

        // Calculate elapsed time in seconds
        double elapsed_time = std::chrono::duration<double>(current_time - start_time).count();

        // If less than 0.5 seconds have passed, set a tone
        if (elapsed_time < 0.5) {
            uint32_t timerPeriod = 1000; // Tone
            setPeriod(&htim7,timerPeriod);
        }
        // If between 0.5 and 1.5 seconds have passed, set no tone
        else if (elapsed_time >= 0.5 && elapsed_time < 1.5) {
            // Silence
            // Set the period to a value that indicates no tone
            // This will depend on your specific hardware and software
            setPeriod(&htim7,5000);
        }
        // Reset the timer after 1.5 seconds
        else if (elapsed_time >= 1.5) {
            start_time = current_time;
        }
    }
}


void
SoundSender::setPeriod(TIM_HandleTypeDef* htim, uint32_t newPeriod)
{
  if (htim != NULL)
  {
    HAL_TIM_Base_Stop(htim);
    htim->Instance->ARR = newPeriod;
    HAL_TIM_Base_Start(htim);
  }
}

void
SoundSender::changeVolume(float vol)
{
	for(int i=0;i<NS;i++){
		*(ptrLUT+i) = (int)(*(ptrLUT+i)*vol);
  }

}

void
SoundSender::sound_disable()
{
    HAL_GPIO_WritePin(AUDIO_ENABLE_GPIO_Port, AUDIO_ENABLE_Pin, GPIO_PIN_SET);
}


void
SoundSender::sound_enable()
{
    HAL_GPIO_WritePin(AUDIO_ENABLE_GPIO_Port, AUDIO_ENABLE_Pin, GPIO_PIN_RESET);
}


extern "C" void 
DMA1_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_dac2);
}





extern "C" int _gettimeofday(struct timeval *__p, void *__tz)
{
    return 0;
}
