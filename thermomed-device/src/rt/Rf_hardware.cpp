#include "rt/Rf_hardware.h"
#include <Arduino.h>
#include "Handset.h"
#include "main.h"

#define DCDC_ADDR                   0x74
#define DCDCREG_REF                 0x00
#define DCDCREG_IOUT_LIMT           0x02
#define DCDCREG_VOUT_SR             0x03
#define DCDCREG_VOUT_FS             0x04
#define DCDCREG_CDC                 0x05
#define DCDCREG_MODE                0x06
#define DCDCREG_STATUS              0x07

#define ADC_VAC1_Pin                GPIO_PIN_2
#define ADC_VAC1_GPIO_Port          GPIOC

#define ADC_VAC2_Pin                GPIO_PIN_3
#define ADC_VAC2_GPIO_Port          GPIOC

#define RF_ADC_FEEDBACK_PHASE       GPIO_PIN_5 // ADC2
#define RF_ADC_PHASE_GPIO_Port      GPIOC

#define RF_ADC_FEEDBACK_VDC         GPIO_PIN_2 //ADC1
#define RF_ADC_VDC_GPIO_Port        GPIOA

#define RF_ADC_FEEDBACK_CURRENT     GPIO_PIN_3  //ADC1
#define RF_ADC_CURRENT_GPIO_Port    GPIOA

#define RF_ENABLE_Pin               GPIO_PIN_4
#define RF_ENABLE_GPIO_Port         GPIOC

#define ADC_12BIT                   4096.0f
#define ADC_VCC                     3.3f

const float Rf_hardware::GAIN_CONTROL_MIN = 0x1F;
const float Rf_hardware::GAIN_CONTROL_MAX = 0x9F;

// Declare a buffer to store ADC data
uint32_t adc_values[2];

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim2;


Rf_hardware::Rf_hardware()
{
    //
}

void 
Rf_hardware::MX_DMA_Init(void)
{
  // Enable the DMA clock
  __HAL_RCC_DMA1_CLK_ENABLE();
  
  // Configure the DMA peripheral
  hdma_adc1.Instance = DMA1_Channel1;
  hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_adc1.Init.Mode = DMA_CIRCULAR;
  hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
  
  // Initialize the DMA
  HAL_DMA_Init(&hdma_adc1);
  
  // Associate the DMA handle with the ADC handle
  __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
  
  // Enable DMA interrupts
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void
Rf_hardware::MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, RF_ENABLE_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : RF_ENABLE_Pin */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RF_ENABLE_GPIO_Port, &GPIO_InitStruct);



}
extern "C" {
void 
HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    adc_values[0] = HAL_ADC_GetValue(hadc);
    adc_values[1] = HAL_ADC_GetValue(hadc);
    rtsys.rt_callback();

}

}

void
Rf_hardware::beginDCDC()
{
    writeToWireOne(DCDC_ADDR,DCDCREG_VOUT_SR,0x03);
    writeToWireOne(DCDC_ADDR,DCDCREG_VOUT_FS,0x83);
    writeToWireOne(DCDC_ADDR,DCDCREG_CDC,0xA0);
    writeToWireOne(DCDC_ADDR,DCDCREG_MODE,0xA8);
    writeToWireOne(DCDC_ADDR,DCDCREG_CDC,0xE0);


}


void 
Rf_hardware::MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1700;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 200;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
void 
Rf_hardware::MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};


  /* Enable ADC clock */
  __HAL_RCC_ADC12_CLK_ENABLE();

  /* Configure ADC */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = ENABLE;
  hadc1.Init.NbrOfDiscConversion = 2;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T2_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

  HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure ADC channel */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 1;
  sConfig.SamplingTime                        = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff                          = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber                        = ADC_OFFSET_NONE;
  sConfig.Offset                              = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure ADC channel */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 2;
  sConfig.SamplingTime                        = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff                          = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber                        = ADC_OFFSET_NONE;
  sConfig.Offset                              = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}


/*** HW interface ***********************************************************/
void
Rf_hardware::pke_enable()
{
    HAL_GPIO_WritePin(RF_ENABLE_GPIO_Port, RF_ENABLE_Pin, GPIO_PIN_SET);
    beginDCDC();
}

void
Rf_hardware::pke_disable()
{
    //writeDCDCOutToWireOne(DCDC_ADDR,DCDCREG_REF,0x00);

    HAL_GPIO_WritePin(RF_ENABLE_GPIO_Port, RF_ENABLE_Pin, GPIO_PIN_RESET);
    
}



/**
 * @brief Read ADC voltage, proportional to current
 * 
 * @return float ADC voltage
*/



/**
 * @brief Set DAC voltage. Must be between 1.0V and 2.9V.
 * 
 * @param v value between 1.0 and 2.9
 */

void
Rf_hardware::set_DCDC_output_hw(byte v)
{
    if (v < GAIN_CONTROL_MIN) {
        v = GAIN_CONTROL_MIN;
    } else if (v > GAIN_CONTROL_MAX) {
        v = GAIN_CONTROL_MAX;
    }

    writeDCDCOutToWireOne(DCDC_ADDR,DCDCREG_REF,v);
}


byte
Rf_hardware::readStat()
{

    return Handset.readfromWireOne(DCDC_ADDR,DCDCREG_STATUS);

}


void
Rf_hardware::begin()
{

   
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_DMA_Init();
    MX_TIM2_Init();
    beginDCDC();

    /* Calibrate w/o start */
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

    

    /* Start TIM2 and ADC */

    if (HAL_TIM_Base_Start(&htim2) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_Start_IT(&hadc1) != HAL_OK) {
        Error_Handler();
    }




}

void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */

float 
Rf_hardware::read_adc_VDC()
{
    uint32_t adc_value = adc_values[0];
    return adc_value*30;
}

float 
Rf_hardware::read_adc_IDC()
{
    uint32_t adc_value = adc_values[1];
    return adc_value/0.4;
}

/*
float 
Rf_hardware::read_adc_phi()
{
    uint32_t adc_value = adc_data[0];
    return adc_value;
}
*/
void 
Rf_hardware::writeToWireOne(byte ADDR,byte REG, byte value)
{
    Handset.writeToWireOne(ADDR,REG,value);
}
        
void 
Rf_hardware::writeDCDCOutToWireOne(byte ADDR,byte REG, byte value)
{
    Handset.writeDCDCOutToWireOne(ADDR,REG,value);

}
        
byte
Rf_hardware::readfromWireOne(byte ADDR,byte REG)
{
     return Handset.readfromWireOne(ADDR,REG);
}

/*** IRQ Handler ************************************************************/
extern "C" void
ADC1_2_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&hadc1);
}
