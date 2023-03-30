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

static TIM_HandleTypeDef            htim2;
static ADC_HandleTypeDef            hadc1;
static ADC_HandleTypeDef            hadc2;


Rf_hardware::Rf_hardware()
{
    //
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
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RF_ENABLE_GPIO_Port, &GPIO_InitStruct);

}

void
Rf_hardware::beginDCDC()
{
    writeToWireOne(DCDC_ADDR,DCDCREG_VOUT_SR,0x03);
    writeToWireOne(DCDC_ADDR,DCDCREG_VOUT_FS,0x03);
    writeToWireOne(DCDC_ADDR,DCDCREG_CDC,0x03);
    writeToWireOne(DCDC_ADDR,DCDCREG_MODE,0x03);
    writeToWireOne(DCDC_ADDR,DCDCREG_CDC,0x03);


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
Rf_hardware::beginADC1()
{

    ADC_MultiModeTypeDef    multimode           = {0};
    GPIO_InitTypeDef        GPIO_InitStruct     = {0};

    /** Common config */
    hadc1.Instance                              = ADC1;
    hadc1.Init.ClockPrescaler                   = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution                       = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign                        = ADC_DATAALIGN_RIGHT;
    hadc1.Init.GainCompensation                 = 0;
    hadc1.Init.ScanConvMode                     = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection                     = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait                 = DISABLE;
    hadc1.Init.ContinuousConvMode               = DISABLE;
    hadc1.Init.NbrOfConversion                  = 1;
    hadc1.Init.DiscontinuousConvMode            = DISABLE;
    hadc1.Init.ExternalTrigConv                 = ADC_EXTERNALTRIG_T2_TRGO;
    hadc1.Init.ExternalTrigConvEdge             = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.DMAContinuousRequests            = DISABLE;
    hadc1.Init.Overrun                          = ADC_OVR_DATA_PRESERVED;
    hadc1.Init.OversamplingMode                 = DISABLE;

    /*** MspInit BEGIN ***/

    /* Peripheral clock enable */
    __HAL_RCC_ADC12_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /** ADC1 GPIO Configuration  */
    GPIO_InitStruct.Pin                         = RF_ADC_FEEDBACK_VDC|RF_ADC_FEEDBACK_CURRENT;
    GPIO_InitStruct.Mode                        = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull                        = GPIO_NOPULL;
    HAL_GPIO_Init(RF_ADC_VDC_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin                         = ADC_VAC1_Pin|ADC_VAC2_Pin;
    GPIO_InitStruct.Mode                        = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull                        = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

    /*** MspInit END ***/
    
    /* uses HAL_ADC_MspInit() internally, but unused! */
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure the ADC multi-mode */
    multimode.Mode                              = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK) {
        Error_Handler();
    }
}


void
Rf_hardware::beginADC2()
{

    ADC_MultiModeTypeDef    multimode           = {0};
    GPIO_InitTypeDef        GPIO_InitStruct     = {0};

    /** Common config */
    hadc2.Instance                              = ADC2;
    hadc2.Init.ClockPrescaler                   = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc2.Init.Resolution                       = ADC_RESOLUTION_12B;
    hadc2.Init.DataAlign                        = ADC_DATAALIGN_RIGHT;
    hadc2.Init.GainCompensation                 = 0;
    hadc2.Init.ScanConvMode                     = ADC_SCAN_DISABLE;
    hadc2.Init.EOCSelection                     = ADC_EOC_SINGLE_CONV;
    hadc2.Init.LowPowerAutoWait                 = DISABLE;
    hadc2.Init.ContinuousConvMode               = DISABLE;
    hadc2.Init.NbrOfConversion                  = 1;
    hadc2.Init.DiscontinuousConvMode            = DISABLE;
    hadc2.Init.ExternalTrigConv                 = ADC_EXTERNALTRIG_T2_TRGO;
    hadc2.Init.ExternalTrigConvEdge             = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc2.Init.DMAContinuousRequests            = DISABLE;
    hadc2.Init.Overrun                          = ADC_OVR_DATA_PRESERVED;
    hadc2.Init.OversamplingMode                 = DISABLE;

    /*** MspInit BEGIN ***/


    /** ADC2 GPIO Configuration  */
    GPIO_InitStruct.Pin                         = RF_ADC_FEEDBACK_PHASE;
    GPIO_InitStruct.Mode                        = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull                        = GPIO_NOPULL;
    HAL_GPIO_Init(RF_ADC_PHASE_GPIO_Port, &GPIO_InitStruct);

    
    /* uses HAL_ADC_MspInit() internally, but unused! */
    if (HAL_ADC_Init(&hadc2) != HAL_OK) {
        Error_Handler();
    }

    /** Configure the ADC multi-mode */
    //multimode.Mode                              = ADC_MODE_INDEPENDENT;
    //if (HAL_ADCEx_MultiModeConfigChannel(&hadc2, &multimode) != HAL_OK) {
    //    Error_Handler();
    //}
}

void
Rf_hardware::ADC_Select_CH4()
{
    ADC_ChannelConfTypeDef  sConfig             = {0};

    sConfig.Channel                             = ADC_CHANNEL_4;
    sConfig.Rank                                = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime                        = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff                          = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber                        = ADC_OFFSET_NONE;
    sConfig.Offset                              = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
}

void
Rf_hardware::ADC_Select_CH11()
{
    ADC_ChannelConfTypeDef  sConfig             = {0};

    sConfig.Channel                             = ADC_CHANNEL_11;
    sConfig.Rank                                = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime                        = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff                          = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber                        = ADC_OFFSET_NONE;
    sConfig.Offset                              = 0;
    if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK) {
        Error_Handler();
    }
}

void
Rf_hardware::ADC_Select_CH3()
{

    ADC_ChannelConfTypeDef  sConfig             = {0};

    sConfig.Channel                             = ADC_CHANNEL_3;
    sConfig.Rank                                = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime                        = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff                          = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber                        = ADC_OFFSET_NONE;
    sConfig.Offset                              = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
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
float
Rf_hardware::read_adc_VDC()  // input = 3.3V @ 30VDC. 
{
    ADC_Select_CH3();
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	float ADC_VAL = ADC_VCC * static_cast<float>(HAL_ADC_GetValue(&hadc1)) / ADC_12BIT;
	HAL_ADC_Stop(&hadc1);

    float V = ADC_VAL*30;
    return V;
}

float
Rf_hardware::read_adc_IDC() // 1.2V @ 3A. Why so low-> DCDCspecs. Still Genauigkeit von 2-3mA. -> Enough
{ 
    ADC_Select_CH4();
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1000);
	float ADC_VAL = ADC_VCC * static_cast<float>(HAL_ADC_GetValue(&hadc1)) / ADC_12BIT;
	HAL_ADC_Stop(&hadc1);

    float I = ADC_VAL/0.4;
    return I;
}

float
Rf_hardware::read_adc_phi() 
{
    ADC_Select_CH11();
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 1000);
	float ADC_VAL = ADC_VCC * static_cast<float>(HAL_ADC_GetValue(&hadc2)) / ADC_12BIT;
	HAL_ADC_Stop(&hadc2);
    return ADC_VAL;
}

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
    beginTimer2();
    beginADC1();
    beginADC2();
    beginGpio();

    /* Calibrate w/o start */
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }
    
    /* Start TIM2 and ADC */
	if (HAL_TIM_Base_Start(&htim2) != HAL_OK) {
        Error_Handler();
    }

	if (HAL_ADC_Start_IT(&hadc1) != HAL_OK) {
        Error_Handler();
    }

	if (HAL_ADC_Start_IT(&hadc2) != HAL_OK) {
        Error_Handler();
    }

}


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
