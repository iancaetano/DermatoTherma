#include "rt/Rf_hardware.h"
#include <Arduino.h>
#include "rt/Flags.h"


#define DEBUG_Pin                   GPIO_PIN_10
#define DEBUG_GPIO_Port             GPIOB

#define ADC_VAC1_Pin                GPIO_PIN_2
#define ADC_VAC1_GPIO_Port          GPIOC

#define ADC_VAC2_Pin                GPIO_PIN_3
#define ADC_VAC2_GPIO_Port          GPIOC

#define RF_ADC_PHI_Pin              GPIO_PIN_1
#define RF_ADC_RFIN_GPIO_Port       GPIOA

#define RF_ADC_I_Pin                GPIO_PIN_2
#define RF_ADC_RFIN_GPIO_Port       GPIOA

#define RF_ADC_VDC_Pin              GPIO_PIN_3
#define RF_ADC_RFIN_GPIO_Port       GPIOA

#define RF_DAC_CONTROL_Pin          GPIO_PIN_4
#define RF_DAC_CONTROL_GPIO_Port    GPIOA

#define RF_ENABLE_Pin               GPIO_PIN_4
#define RF_ENABLE_GPIO_Port         GPIOC


#define ADC_12BIT                   4096.0f
#define ADC_VCC                     3.3f

uint32_t adc_values[3];
uint32_t lastADCValue[3] = {0};
bool firstReadI = 1;

const float Rf_hardware::GAIN_CONTROL_MIN = 0.0f;
const float Rf_hardware::GAIN_CONTROL_MAX = 1.5f;

static TIM_HandleTypeDef            htim2;
static ADC_HandleTypeDef            hadc1;
static DAC_HandleTypeDef            hdac1;
static DMA_HandleTypeDef            hdma_adc1;

Rf_hardware::Rf_hardware()
{
    //
}

extern "C" {

/*
void
HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{ 
    if (hadc==&hadc1){
        
    }
}
*/
};
void Rf_hardware::beginTimer2()
{
    TIM_ClockConfigTypeDef  sClockSourceConfig  = {0};
    TIM_MasterConfigTypeDef sMasterConfig       = {0};

    /* 170 MHz / 1700 = 100 kHz */
    /* 100 kHz / 200 = 500 Hz = 2 ms */
    htim2.Instance                              = TIM2;
    htim2.Init.Prescaler                        = 65535;
    htim2.Init.CounterMode                      = TIM_COUNTERMODE_UP;
    htim2.Init.Period                           = 500;
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
Rf_hardware::beginADC1()
{

    ADC_MultiModeTypeDef    multimode           = {0};
    GPIO_InitTypeDef        GPIO_InitStruct     = {0};

    ADC_ChannelConfTypeDef sConfig1;
    ADC_ChannelConfTypeDef sConfig2;
    ADC_ChannelConfTypeDef sConfig3;

    /** Common config */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T2_TRGO;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
    /* uses HAL_ADC_MspInit() internally, but unused! */
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }


    /*** MspInit BEGIN ***/

    /* Peripheral clock enable */
    __HAL_RCC_ADC12_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /** ADC1 GPIO Configuration  */
    GPIO_InitStruct.Pin                         = ADC_VAC1_Pin|ADC_VAC2_Pin;
    GPIO_InitStruct.Mode                        = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull                        = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin                         = RF_ADC_VDC_Pin|RF_ADC_I_Pin|RF_ADC_PHI_Pin;
    GPIO_InitStruct.Mode                        = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull                        = GPIO_NOPULL;
    HAL_GPIO_Init(RF_ADC_RFIN_GPIO_Port, &GPIO_InitStruct);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /*** MspInit END ***/
    

    /* DMA controller clock enable */
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

/* ADC1 DMA Init */
/* ADC1 Init */
    hdma_adc1.Instance = DMA1_Channel1;
    hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_MEDIUM;

    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
        Error_Handler();
    }



    /** Configure the ADC multi-mode */
    multimode.Mode                              = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK) {
        Error_Handler();
    }

/** Configure Channel
  */
  sConfig1.Channel = ADC_CHANNEL_2;
  sConfig1.Rank = ADC_REGULAR_RANK_1;
  sConfig1.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
  sConfig1.SingleDiff = ADC_SINGLE_ENDED;
  sConfig1.OffsetNumber = ADC_OFFSET_NONE;
  sConfig1.Offset = 0;

  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig2.Channel = ADC_CHANNEL_3;
  sConfig2.Rank = ADC_REGULAR_RANK_2;
  sConfig2.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
  sConfig2.SingleDiff = ADC_SINGLE_ENDED;
  sConfig2.OffsetNumber = ADC_OFFSET_NONE;
  sConfig2.Offset = 0;

  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig2) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig3.Channel = ADC_CHANNEL_4;
  sConfig3.Rank = ADC_REGULAR_RANK_3;
  sConfig3.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
  sConfig3.SingleDiff = ADC_SINGLE_ENDED;
  sConfig3.OffsetNumber = ADC_OFFSET_NONE;
  sConfig3.Offset = 0;

  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig3) != HAL_OK)
  {
    Error_Handler();
  }
}

void
Rf_hardware::beginDAC1()
{
    DAC_ChannelConfTypeDef  sConfig             = {0};
    GPIO_InitTypeDef        GPIO_InitStruct     = {0};

    /*** MspInit BEGIN ***/

    /* Peripheral clock enable */
    __HAL_RCC_DAC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /** DAC1 GPIO Configuration */
    GPIO_InitStruct.Pin                         = RF_DAC_CONTROL_Pin;
    GPIO_InitStruct.Mode                        = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull                        = GPIO_NOPULL;
    HAL_GPIO_Init(RF_DAC_CONTROL_GPIO_Port, &GPIO_InitStruct);

    /*** MspInit END ***/

    /** DAC Initialization */
    hdac1.Instance = DAC1;
    if (HAL_DAC_Init(&hdac1) != HAL_OK) {
        Error_Handler();
    }

    /** DAC channel OUT1 config */
    sConfig.DAC_HighFrequency                   = DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC;
    sConfig.DAC_DMADoubleDataMode               = DISABLE;
    sConfig.DAC_SignedFormat                    = DISABLE;
    sConfig.DAC_SampleAndHold                   = DAC_SAMPLEANDHOLD_DISABLE;
    sConfig.DAC_Trigger                         = DAC_TRIGGER_NONE;
    sConfig.DAC_Trigger2                        = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer                    = DAC_OUTPUTBUFFER_ENABLE;
    sConfig.DAC_ConnectOnChipPeripheral         = DAC_CHIPCONNECT_EXTERNAL;
    sConfig.DAC_UserTrimming                    = DAC_TRIMMING_FACTORY;
    if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK) {
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
    HAL_GPIO_WritePin(GPIOC, RF_ENABLE_Pin, GPIO_PIN_RESET);


    /*Configure GPIO pins : RF_ENABLE_Pin */
    GPIO_InitStruct.Pin = RF_ENABLE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RF_ENABLE_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DEBUG_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = 0; // macroname?
    HAL_GPIO_Init(DEBUG_GPIO_Port, &GPIO_InitStruct);

}

void
Rf_hardware::begin()
{
    beginGpio();
    beginDAC1();
    beginADC1();
    beginTimer2();
    

    /* Calibrate w/o start */
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

	if (HAL_TIM_Base_Start(&htim2) != HAL_OK) {
        Error_Handler();
    }

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_values, 3) != HAL_OK) {
    Error_Handler();
    }


    /* Start DAC1 */
    if (HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
}


/*** HW interface ***********************************************************/
void
Rf_hardware::pke_enable()
{
    HAL_GPIO_WritePin(RF_ENABLE_GPIO_Port, RF_ENABLE_Pin, GPIO_PIN_SET);
}

void
Rf_hardware::pke_disable()
{
    HAL_GPIO_WritePin(RF_ENABLE_GPIO_Port, RF_ENABLE_Pin, GPIO_PIN_RESET);
}



/**
 * @brief Read ADC voltage, proportional to current
 * 
 * @return float ADC voltage
 */


float
Rf_hardware::read_adc_VDC()
{  
    float BridgeGain = 4.6;
    uint32_t adc_value = adc_values[2];
    float vdc = ADC_VCC/ADC_12BIT*adc_value*BridgeGain; 
    return vdc;
}

float 
Rf_hardware::read_adc_IDC()
{

    float Rshunt = 0.1;
    float AmpGain = 10;
    float Ioffset = 0.5;
    float idc = (ADC_VCC/ADC_12BIT*adc_values[1]-Ioffset)/Rshunt/AmpGain;
    dummy_IDC = idc;
    return idc;
}

float 
Rf_hardware::read_adc_phi()
{
    uint32_t adc_value = adc_values[0];
    return adc_value;
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
    
    value = static_cast<uint32_t>(ADC_12BIT * (1-v/ ADC_VCC));
    
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
}

/*** IRQ Handler ************************************************************/
extern "C" void
ADC1_2_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&hadc1);
}

extern "C" void
DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_adc1);
  RTcallbackFlag = 1;
}

