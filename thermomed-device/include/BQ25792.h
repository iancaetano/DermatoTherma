#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <inttypes.h>

#include "SoftwareTimer.h"

class BQ25792 : public SoftwareTimerHandler {

    private: 
        typedef uint8_t     RegAddress;
        typedef uint8_t     RegSize;
        typedef uint32_t    Reg;
        typedef uint8_t     FieldMask;
        typedef uint8_t     FieldPosition;
        typedef uint32_t    FieldOffset;
        typedef uint32_t    FieldBitStep;


        typedef void (*dissector_func)(byte data[], byte size);

        typedef struct {
            byte            address;
            const char     *name;
            byte            size;
            dissector_func  func;
        } RegDissector;

    private:
        static const byte       I2C_ADDRESS;
        static const byte       I2C_DATA_SIZE_MAX;
        static const byte       REG_SIZE;

        static const RegAddress Minimal_System_Voltage;
        static const RegAddress Charge_Voltage_Limit;
        static const RegAddress Charge_Current_Limit;
        static const RegAddress Input_Voltage_Limit;
        static const RegAddress Input_Current_Limit;
        static const RegAddress Precharge_Control;
        static const RegAddress Termination_Control;
        static const RegAddress Recharge_Control;
        static const RegAddress VOTG_regulation;
        static const RegAddress IOTG_regulation;
        static const RegAddress Timer_Control;
        static const RegAddress Charger_Control;
        static const RegAddress Temperature_Control ;
        static const RegAddress NTC_Control;
        static const RegAddress ICO_Current_Limit;
        static const RegAddress Charger_Status;
        static const RegAddress FAULT_Status;
        static const RegAddress Charger_Flag;
        static const RegAddress FAULT_Flag;
        static const RegAddress Charger_Mask;
        static const RegAddress FAULT_Mask;
        static const RegAddress ADC_Control;
        static const RegAddress ADC_Function_Disable;
        static const RegAddress IBUS_ADC;
        static const RegAddress IBAT_ADC;
        static const RegAddress VBUS_ADC;
        static const RegAddress VAC1_ADC;
        static const RegAddress VAC2_ADC;
        static const RegAddress VBAT_ADC;
        static const RegAddress VSYS_ADC;
        static const RegAddress TS_ADC;
        static const RegAddress TDIE_ADC;
        static const RegAddress DP_ADC;
        static const RegAddress DN_ADC;
        static const RegAddress DPDM_Driver;
        static const RegAddress Part_Information;

        static const RegSize Minimal_System_Voltage_Size    = 1;
        static const RegSize Charge_Voltage_Limit_Size      = 2;
        static const RegSize Charge_Current_Limit_Size      = 2;
        static const RegSize Input_Voltage_Limit_Size       = 1;
        static const RegSize Input_Current_Limit_Size       = 2;
        static const RegSize Precharge_Control_Size         = 1;
        static const RegSize Termination_Control_Size       = 1;
        static const RegSize Recharge_Control_Size          = 1;
        static const RegSize VOTG_regulation_Size           = 2;
        static const RegSize IOTG_regulation_Size           = 1;
        static const RegSize Timer_Control_Size             = 1;
        static const RegSize Charger_Control_Size           = 6;
        static const RegSize Temperature_Control_Size       = 1;
        static const RegSize NTC_Control_Size               = 2;
        static const RegSize ICO_Current_Limit_Size         = 2;
        static const RegSize Charger_Status_Size            = 5;
        static const RegSize FAULT_Status_Size              = 2;
        static const RegSize Charger_Flag_Size              = 4;
        static const RegSize FAULT_Flag_Size                = 2;
        static const RegSize Charger_Mask_Size              = 4;
        static const RegSize FAULT_Mask_Size                = 2;
        static const RegSize ADC_Control_Size               = 1;
        static const RegSize ADC_Function_Disable_Size      = 2;
        static const RegSize IBUS_ADC_Size                  = 2;
        static const RegSize IBAT_ADC_Size                  = 2;
        static const RegSize VBUS_ADC_Size                  = 2;
        static const RegSize VAC1_ADC_Size                  = 2;
        static const RegSize VAC2_ADC_Size                  = 2;
        static const RegSize VBAT_ADC_Size                  = 2;
        static const RegSize VSYS_ADC_Size                  = 2;
        static const RegSize TS_ADC_Size                    = 2;
        static const RegSize TDIE_ADC_Size                  = 2;
        static const RegSize DP_ADC_Size                    = 2;
        static const RegSize DN_ADC_Size                    = 2;
        static const RegSize DPDM_Driver_Size               = 1;
        static const RegSize Part_Information_Size          = 1;

        static const RegDissector   regDissector[];

        static const FieldBitStep   mV_to_V;
        
        static const FieldMask      VSYSMIN_Mask;
        static const FieldBitStep   VSYSMIN_BitStep;
        static const FieldOffset    VSYSMIN_Offset;
        
        static const FieldMask      VREG_Mask;
        static const FieldBitStep   VREG_BitStep;
        static const FieldOffset    VREG_Offset;
        
        static const FieldMask      ICHG_Mask;
        static const FieldBitStep   ICHG_BitStep;
        static const FieldOffset    ICHG_Offset;

        static const FieldBitStep   VINDPM_BitStep;
        static const FieldOffset    VINDPM_Offset;

        static const FieldMask      IINDPM_Mask;
        static const FieldBitStep   IINDPM_BitStep;
        static const FieldOffset    IINDPM_Offset;

        static const FieldMask      REG_RST_Mask;
        static const FieldMask      ITERM_Mask;
        static const FieldPosition  REG_RST_Position;
        static const FieldPosition  ITERM_Position;
        static const FieldBitStep   ITERM_BitStep;
        static const FieldOffset    ITERM_Offset;

        /* Charger Control */

        /* [0] */
        static const FieldMask      EN_AUTO_IBATDIS_Mask;
        static const FieldMask      FORCE_IBATDIS_Mask;
        static const FieldMask      EN_CHG_Mask;
        static const FieldMask      EN_ICO_Mask;
        static const FieldMask      FORCE_ICO_Mask;
        static const FieldMask      EN_HIZ_Mask;
        static const FieldMask      EN_TERM_Mask;
        static const FieldPosition  EN_AUTO_IBATDIS_Position;
        static const FieldPosition  FORCE_IBATDIS_Position;
        static const FieldPosition  EN_CHG_Position;
        static const FieldPosition  EN_ICO_Position;
        static const FieldPosition  FORCE_ICO_Position;
        static const FieldPosition  EN_HIZ_Position;
        static const FieldPosition  EN_TERM_Position;

        /* [1] */
        static const FieldMask      VAC_OVP_Mask;
        static const FieldMask      WD_RST_Mask;
        static const FieldMask      WATCHDOG_Mask;
        static const FieldPosition  VAC_OVP_Position;
        static const FieldPosition  WD_RST_Position;
        static const FieldPosition  WATCHDOG_Position;

        /* [2] */
        static const FieldMask      FORCE_INDET_Mask;
        static const FieldMask      AUTO_INDET_EN_Mask;
        static const FieldMask      EN_12V_Mask;
        static const FieldMask      EN_9V_Mask;
        static const FieldMask      HVDCP_EN_Mask;
        static const FieldMask      SDRV_CTRL_Mask;
        static const FieldMask      SDRV_DLY_Mask;
        static const FieldPosition  FORCE_INDET_Position;
        static const FieldPosition  AUTO_INDET_EN_Position;
        static const FieldPosition  EN_12V_Position;
        static const FieldPosition  EN_9V_Position;
        static const FieldPosition  HVDCP_EN_Position;
        static const FieldPosition  SDRV_CTRL_Position;
        static const FieldPosition  SDRV_DLY_Position;

        /* [3] */
        static const FieldMask      DIS_ACDRV_Mask;
        static const FieldMask      EN_OTG_Mask;
        static const FieldMask      PFM_OTG_DIS_Mask;
        static const FieldMask      PFM_FWD_DIS_Mask;
        static const FieldMask      WKUP_DLY_Mask;
        static const FieldMask      DIS_LDO_Mask;
        static const FieldMask      DIS_OTG_OOA_Mask;
        static const FieldMask      DIS_FWD_OOA_Mask;
        static const FieldPosition  DIS_ACDRV_Position;
        static const FieldPosition  EN_OTG_Position;
        static const FieldPosition  PFM_OTG_DIS_Position;
        static const FieldPosition  PFM_FWD_DIS_Position;
        static const FieldPosition  WKUP_DLY_Position;
        static const FieldPosition  DIS_LDO_Position;
        static const FieldPosition  DIS_OTG_OOA_Position;
        static const FieldPosition  DIS_FWD_OOA_Position;

        /* [4] */
        static const FieldMask      EN_ACDRV2_Mask;
        static const FieldMask      EN_ACDRV1_Mask;
        static const FieldMask      PWM_FREQ_Mask;
        static const FieldMask      DIS_STAT_Mask;
        static const FieldMask      DIS_VSYS_SHORT_Mask;
        static const FieldMask      DIS_VOTG_UVP_Mask;
        static const FieldMask      FORCE_VINDPM_DET_Mask;
        static const FieldMask      EN_IBUS_OCP_Mask;
        static const FieldPosition  EN_ACDRV2_Position;
        static const FieldPosition  EN_ACDRV1_Position;
        static const FieldPosition  PWM_FREQ_Position;
        static const FieldPosition  DIS_STAT_Position;
        static const FieldPosition  DIS_VSYS_SHORT_Position;
        static const FieldPosition  DIS_VOTG_UVP_Position;
        static const FieldPosition  FORCE_VINDPM_DET_Position;
        static const FieldPosition  EN_IBUS_OCP_Position;

        /* [5] */
        static const FieldMask      SFET_PRESENT_Mask;
        static const FieldMask      EN_IBAT_Mask;
        static const FieldMask      IBAT_REG_Mask;
        static const FieldMask      EN_IINDPM_Mask;
        static const FieldMask      EN_EXTILIM_Mask;
        static const FieldMask      EN_BATOC_Mask;
        static const FieldPosition  SFET_PRESENT_Position;
        static const FieldPosition  EN_IBAT_Position;
        static const FieldPosition  IBAT_REG_Position;
        static const FieldPosition  EN_IINDPM_Position;
        static const FieldPosition  EN_EXTILIM_Position;
        static const FieldPosition  EN_BATOC_Position;

        /* Charger Status */

        /* [0] */
        static const FieldMask      IINDPM_STAT_Mask;
        static const FieldMask      VINDPM_STAT_Mask;
        static const FieldMask      WD_STAT_Mask;
        static const FieldMask      POORSRC_STAT_Mask;
        static const FieldMask      PG_STAT_Mask;
        static const FieldMask      AC2_PRESENT_STAT_Mask;
        static const FieldMask      AC1_PRESENT_STAT_Mask;
        static const FieldMask      VBUS_PRESENT_STAT_Mask;
        static const FieldPosition  IINDPM_STAT_Position;
        static const FieldPosition  VINDPM_STAT_Position;
        static const FieldPosition  WD_STAT_Position;
        static const FieldPosition  POORSRC_STAT_Position;
        static const FieldPosition  PG_STAT_Position;
        static const FieldPosition  AC2_PRESENT_STAT_Position;
        static const FieldPosition  AC1_PRESENT_STAT_Position;
        static const FieldPosition  VBUS_PRESENT_STAT_Position;

        /* [1] */
        static const FieldMask      CHG_STAT_Mask;
        static const FieldMask      VBUS_STAT_Mask;
        static const FieldMask      BC12_DONE_STAT_Mask;
        static const FieldPosition  CHG_STAT_Position;
        static const FieldPosition  VBUS_STAT_Position;
        static const FieldPosition  BC12_DONE_STAT_Position;

        /* [2] */
        static const FieldMask      ICO_STAT_Mask;
        static const FieldMask      TREG_STAT_Mask;
        static const FieldMask      DPDM_STAT_Mask;
        static const FieldMask      VBAT_PRESENT_STAT_Mask;
        static const FieldPosition  ICO_STAT_Position;
        static const FieldPosition  TREG_STAT_Position;
        static const FieldPosition  DPDM_STAT_Position;
        static const FieldPosition  VBAT_PRESENT_STAT_Position;

        /* [3] */
        static const FieldMask      ACRB2_STAT_Mask;
        static const FieldMask      ACRB1_STAT_Mask;
        static const FieldMask      ADC_DONE_STAT_Mask;
        static const FieldMask      VSYS_STAT_Mask;
        static const FieldMask      CHG_TMR_STAT_Mask;
        static const FieldMask      TRICHG_TMR_STAT_Mask;
        static const FieldMask      PRECHG_TMR_STAT_Mask;
        static const FieldPosition  ACRB2_STAT_Position;
        static const FieldPosition  ACRB1_STAT_Position;
        static const FieldPosition  ADC_DONE_STAT_Position;
        static const FieldPosition  VSYS_STAT_Position;
        static const FieldPosition  CHG_TMR_STAT_Position;
        static const FieldPosition  TRICHG_TMR_STAT_Position;
        static const FieldPosition  PRECHG_TMR_STAT_Position;

        /* [4] */
        static const FieldMask      VBATOTG_LOW_STAT_Mask;
        static const FieldMask      TS_COLD_STAT_Mask;
        static const FieldMask      TS_COOL_STAT_Mask;
        static const FieldMask      TS_WARM_STAT_Mask;
        static const FieldMask      TS_HOT_STAT_Mask;
        static const FieldPosition  VBATOTG_LOW_STAT_Position;
        static const FieldPosition  TS_COLD_STAT_Position;
        static const FieldPosition  TS_COOL_STAT_Position;
        static const FieldPosition  TS_WARM_STAT_Position;
        static const FieldPosition  TS_HOT_STAT_Position;


        /* Charger Flag */

        /* [0] */
        static const FieldMask      IINDPM_FLAG_Mask;
        static const FieldMask      VINDPM_FLAG_Mask;
        static const FieldMask      WD_FLAG_Mask;
        static const FieldMask      POORSRC_FLAG_Mask;
        static const FieldMask      PG_FLAG_Mask;
        static const FieldMask      AC2_PRESENT_FLAG_Mask;
        static const FieldMask      AC1_PRESENT_FLAG_Mask;
        static const FieldMask      VBUS_PRESENT_FLAG_Mask;
        static const FieldPosition  IINDPM_FLAG_Position;
        static const FieldPosition  VINDPM_FLAG_Position;
        static const FieldPosition  WD_FLAG_Position;
        static const FieldPosition  POORSRC_FLAG_Position;
        static const FieldPosition  PG_FLAG_Position;
        static const FieldPosition  AC2_PRESENT_FLAG_Position;
        static const FieldPosition  AC1_PRESENT_FLAG_Position;
        static const FieldPosition  VBUS_PRESENT_FLAG_Position;

        /* [1] */
        static const FieldMask      CHG_FLAG_Mask;
        static const FieldMask      ICO_FLAG_Mask;
        static const FieldMask      VBUS_FLAG_Mask;
        static const FieldMask      TREG_FLAG_Mask;
        static const FieldMask      VBAT_PRESENT_FLAG_Mask;
        static const FieldMask      BC12_DONE_FLAG_Mask;
        static const FieldPosition  CHG_FLAG_Position;
        static const FieldPosition  ICO_FLAG_Position;
        static const FieldPosition  VBUS_FLAG_Position;
        static const FieldPosition  TREG_FLAG_Position;
        static const FieldPosition  VBAT_PRESENT_FLAG_Position;
        static const FieldPosition  BC12_DONE_FLAG_Position;

        /* [2] */
        static const FieldMask      DPDM_DONE_FLAG_Mask;
        static const FieldMask      ADC_DONE_FLAG_Mask;
        static const FieldMask      VSYS_FLAG_Mask;
        static const FieldMask      CHG_TMR_FLAG_Mask;
        static const FieldMask      TRICHG_TMR_FLAG_Mask;
        static const FieldMask      PRECHG_TMR_FLAG_Mask;
        static const FieldMask      TOPOFF_TMR_FLAG_Mask;
        static const FieldPosition  DPDM_DONE_FLAG_Position;
        static const FieldPosition  ADC_DONE_FLAG_Position;
        static const FieldPosition  VSYS_FLAG_Position;
        static const FieldPosition  CHG_TMR_FLAG_Position;
        static const FieldPosition  TRICHG_TMR_FLAG_Position;
        static const FieldPosition  PRECHG_TMR_FLAG_Position;
        static const FieldPosition  TOPOFF_TMR_FLAG_Position;

        /* [3] */
        static const FieldMask      VBATOTG_LOW_FLAG_Mask;
        static const FieldMask      TS_COLD_FLAG_Mask;
        static const FieldMask      TS_COOL_FLAG_Mask;
        static const FieldMask      TS_WARM_FLAG_Mask;
        static const FieldMask      TS_HOT_FLAG_Mask;
        static const FieldPosition  VBATOTG_LOW_FLAG_Position;
        static const FieldPosition  TS_COLD_FLAG_Position;
        static const FieldPosition  TS_COOL_FLAG_Position;
        static const FieldPosition  TS_WARM_FLAG_Position;
        static const FieldPosition  TS_HOT_FLAG_Position;

        /* ADC Control */
        static const FieldMask      ADC_EN_Mask;
        static const FieldMask      ADC_RATE_Mask;
        static const FieldMask      ADC_SAMPLE_Mask;
        static const FieldMask      ADC_AVG_Mask;
        static const FieldMask      ADC_AVG_INIT_Mask;
        static const FieldPosition  ADC_EN_Position;
        static const FieldPosition  ADC_RATE_Position;
        static const FieldPosition  ADC_SAMPLE_Position;
        static const FieldPosition  ADC_AVG_Position;
        static const FieldPosition  ADC_AVG_INIT_Position;

        static const double         TS_ADC_BitStep;
        static const FieldOffset    TS_ADC_Offset;

        static void dissect_Minimal_System_Voltage(byte data[], byte size);
        static void dissect_Charge_Voltage_Limit(byte data[], byte size);
        static void dissect_Charge_Current_Limit(byte data[], byte size);
        static void dissect_Input_Voltage_Limit(byte data[], byte size);
        static void dissect_Input_Current_Limit(byte data[], byte size);
        static void dissect_Termination_Control(byte data[], byte size);
        static void dissect_Charger_Control(byte data[], byte size);
        static void dissect_Charger_Status(byte data[], byte size);
        static void dissect_Charger_Flag(byte data[], byte size);
        static void dissect_ADC_Control(byte data[], byte size);
        static void dissect_IBUS_ADC(byte data[], byte size);
        static void dissect_IBAT_ADC(byte data[], byte size);
        static void dissect_VBUS_ADC(byte data[], byte size);
        static void dissect_VAC1_ADC(byte data[], byte size);
        static void dissect_VAC2_ADC(byte data[], byte size);
        static void dissect_VBAT_ADC(byte data[], byte size);
        static void dissect_VSYS_ADC(byte data[], byte size);
        static void dissect_TS_ADC(byte data[], byte size);


    public:

        /* Reg */
        typedef union {
            byte raw[Charger_Status_Size];
            struct {
                /* [0] */
                byte VBUS_PRESENT_STAT      : 1;
                byte AC1_PRESENT_STAT       : 1;
                byte AC2_PRESENT_STAT       : 1;
                byte PG_STAT                : 1;
                byte POORSRC_STAT           : 1;
                byte WD_STAT                : 1;
                byte VINDPM_STAT            : 1;
                byte IINDPM_STAT            : 1;

                /* [1] */
                byte BC12_DONE_STAT         : 1;
                byte VBUS_STAT              : 4;
                byte CHG_STAT               : 3;

                /* [2] */
                byte VBAT_PRESENT_STAT      : 1;
                byte DPDM_STAT              : 1;
                byte TREG_STAT              : 1;
                byte Reserverd2_1           : 3;
                byte ICO_STAT               : 2;

                /* [3] */
                byte Reserved3_1            : 1;
                byte PRECHG_TMR_STAT        : 1;
                byte TRICHG_TMR_STAT        : 1;
                byte CHG_TMR_STAT           : 1;
                byte VSYS_STAT              : 1;
                byte ADC_DONE_STAT          : 1;
                byte ACRB1_STAT             : 1;
                byte ACRB2_STAT             : 1;

                /* [4] */
                byte TS_HOT_STAT            : 1;
                byte TS_WARM_STAT           : 1;
                byte TS_COOL_STAT           : 1;
                byte TS_COLD_STAT           : 1;
                byte VBATOTG_LOW_STAT       : 1;
                byte Reserved4_1            : 3;
            };
        } Reg_Charger_Status;

    public:

        void begin();

        void FIELD_bit(byte reg, byte reset, byte offset, byte data);
        void FIELD_word(byte reg, uint16_t reset, uint16_t offset, uint16_t data);
        void IINDPM(uint16_t data);
        void REG_RST(byte data);
        void WD_RST(byte data);
        void WATCHDOG(byte data);
        void HVDCP_EN(byte data);
        void SDRV_CTRL(byte data);
        void SDRV_DLY(byte data);
        void WKUP_DLY(byte data);
        void SFET_PRESENT(byte data);
        void EN_IBAT(byte data);
        void EN_EXTILIM(byte data);
        void ADC_EN(byte data);
        void ADC_RATE(byte data);

        int getRegister(RegAddress addr, RegSize size, byte data[]);
        int get_Charger_Status(Reg_Charger_Status *charger_status);
        int get_VBAT_ADC(uint16_t *vbat_adc);

        void printRegister();
        void sendBytes(byte address, byte reg, byte *data, byte size);
        void scanDevices();

    /* Main-loop */
    public:
        void loop();
    
    /* SoftwareTimerHandler in Interrupt mode */
    private:
        bool m_timeout;

    public:
        virtual void handle();

};

extern BQ25792 bq25792;

void bq25792_handler();