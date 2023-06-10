

#pragma once



class SoundSender {

private:
bool beepFlag = false;
void MX_DMA_Init(void);
void MX_DAC2_Init(void);
void MX_TIM7_Init(void);
void MX_GPIO_Init(void);
void ErrorTone();
void TreatmentTone();
void setPeriod(TIM_HandleTypeDef* htim, uint32_t newPeriod);

bool isSoundOn;

public:
void loop();
bool getBeepFlag() const {
    return beepFlag;
}
void begin();
void TreatmentTone(float temp);
void changeVolume(float vol);
void sound_disable();
void sound_enable();
bool isSoundEnabled();

};