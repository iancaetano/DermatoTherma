
#include "BQ25792.h"
#include "SerialUtils.h"
#include "main.h"

const BQ25792::RegAddress       BQ25792::Minimal_System_Voltage     = 0x00;
const BQ25792::RegAddress       BQ25792::Charge_Voltage_Limit       = 0x01;
const BQ25792::RegAddress       BQ25792::Charge_Current_Limit       = 0x03;
const BQ25792::RegAddress       BQ25792::Input_Voltage_Limit        = 0x05;
const BQ25792::RegAddress       BQ25792::Input_Current_Limit        = 0x06;
const BQ25792::RegAddress       BQ25792::Precharge_Control          = 0x08;
const BQ25792::RegAddress       BQ25792::Termination_Control        = 0x09;
const BQ25792::RegAddress       BQ25792::Recharge_Control           = 0x0a;
const BQ25792::RegAddress       BQ25792::VOTG_regulation            = 0x0b;
const BQ25792::RegAddress       BQ25792::IOTG_regulation            = 0x0d;
const BQ25792::RegAddress       BQ25792::Timer_Control              = 0x0e;
const BQ25792::RegAddress       BQ25792::Charger_Control            = 0x0f;
const BQ25792::RegAddress       BQ25792::Temperature_Control        = 0x16;
const BQ25792::RegAddress       BQ25792::NTC_Control                = 0x17;
const BQ25792::RegAddress       BQ25792::ICO_Current_Limit          = 0x19;
const BQ25792::RegAddress       BQ25792::Charger_Status             = 0x1b;
const BQ25792::RegAddress       BQ25792::FAULT_Status               = 0x20;
const BQ25792::RegAddress       BQ25792::Charger_Flag               = 0x22;
const BQ25792::RegAddress       BQ25792::FAULT_Flag                 = 0x26;
const BQ25792::RegAddress       BQ25792::Charger_Mask               = 0x28;
const BQ25792::RegAddress       BQ25792::FAULT_Mask                 = 0x2c;
const BQ25792::RegAddress       BQ25792::ADC_Control                = 0x2e;
const BQ25792::RegAddress       BQ25792::ADC_Function_Disable       = 0x2f;
const BQ25792::RegAddress       BQ25792::IBUS_ADC                   = 0x31;
const BQ25792::RegAddress       BQ25792::IBAT_ADC                   = 0x33;
const BQ25792::RegAddress       BQ25792::VBUS_ADC                   = 0x35;
const BQ25792::RegAddress       BQ25792::VAC1_ADC                   = 0x37;
const BQ25792::RegAddress       BQ25792::VAC2_ADC                   = 0x39;
const BQ25792::RegAddress       BQ25792::VBAT_ADC                   = 0x3b;
const BQ25792::RegAddress       BQ25792::VSYS_ADC                   = 0x3d;
const BQ25792::RegAddress       BQ25792::TS_ADC                     = 0x3f;
const BQ25792::RegAddress       BQ25792::TDIE_ADC                   = 0x41;
const BQ25792::RegAddress       BQ25792::DP_ADC                     = 0x43;
const BQ25792::RegAddress       BQ25792::DN_ADC                     = 0x45;
const BQ25792::RegAddress       BQ25792::DPDM_Driver                = 0x47;
const BQ25792::RegAddress       BQ25792::Part_Information           = 0x48;

const BQ25792::FieldBitStep     BQ25792::mV_to_V                    = 1000;

const BQ25792::FieldMask        BQ25792::VSYSMIN_Mask               = 0x3f;
const BQ25792::FieldBitStep     BQ25792::VSYSMIN_BitStep            = 250;
const BQ25792::FieldOffset      BQ25792::VSYSMIN_Offset             = 2500;

const BQ25792::FieldMask        BQ25792::VREG_Mask                  = 0x07;
const BQ25792::FieldBitStep     BQ25792::VREG_BitStep               = 10;
const BQ25792::FieldOffset      BQ25792::VREG_Offset                = 0;

const BQ25792::FieldMask        BQ25792::ICHG_Mask                  = 0x01;
const BQ25792::FieldBitStep     BQ25792::ICHG_BitStep               = 10;
const BQ25792::FieldOffset      BQ25792::ICHG_Offset                = 0;

const BQ25792::FieldBitStep     BQ25792::VINDPM_BitStep             = 100;
const BQ25792::FieldOffset      BQ25792::VINDPM_Offset              = 0;

const BQ25792::FieldMask        BQ25792::IINDPM_Mask                = 0x01;
const BQ25792::FieldBitStep     BQ25792::IINDPM_BitStep             = 10;
const BQ25792::FieldOffset      BQ25792::IINDPM_Offset              = 0;

const BQ25792::FieldMask        BQ25792::REG_RST_Mask               = 0b01000000;
const BQ25792::FieldMask        BQ25792::ITERM_Mask                 = 0b00011111;
const BQ25792::FieldPosition    BQ25792::REG_RST_Position           = 6;
const BQ25792::FieldPosition    BQ25792::ITERM_Position             = 0;
const BQ25792::FieldBitStep     BQ25792::ITERM_BitStep              = 40;
const BQ25792::FieldOffset      BQ25792::ITERM_Offset               = 0;

/* Charger Control */

/* [0] */
const BQ25792::FieldMask        BQ25792::EN_AUTO_IBATDIS_Mask       = 0b10000000;
const BQ25792::FieldMask        BQ25792::FORCE_IBATDIS_Mask         = 0b01000000;
const BQ25792::FieldMask        BQ25792::EN_CHG_Mask                = 0b00100000;
const BQ25792::FieldMask        BQ25792::EN_ICO_Mask                = 0b00010000;
const BQ25792::FieldMask        BQ25792::FORCE_ICO_Mask             = 0b00001000;
const BQ25792::FieldMask        BQ25792::EN_HIZ_Mask                = 0b00000100;
const BQ25792::FieldMask        BQ25792::EN_TERM_Mask               = 0b00000010;
const BQ25792::FieldPosition    BQ25792::EN_AUTO_IBATDIS_Position   = 7;
const BQ25792::FieldPosition    BQ25792::FORCE_IBATDIS_Position     = 6;
const BQ25792::FieldPosition    BQ25792::EN_CHG_Position            = 5;
const BQ25792::FieldPosition    BQ25792::EN_ICO_Position            = 4;
const BQ25792::FieldPosition    BQ25792::FORCE_ICO_Position         = 3;
const BQ25792::FieldPosition    BQ25792::EN_HIZ_Position            = 2;
const BQ25792::FieldPosition    BQ25792::EN_TERM_Position           = 1;

/* [1] */
const BQ25792::FieldMask        BQ25792::VAC_OVP_Mask               = 0b00110000;
const BQ25792::FieldMask        BQ25792::WD_RST_Mask                = 0b00001000;
const BQ25792::FieldMask        BQ25792::WATCHDOG_Mask              = 0b00000111;
const BQ25792::FieldPosition    BQ25792::VAC_OVP_Position           = 4;
const BQ25792::FieldPosition    BQ25792::WD_RST_Position            = 3;
const BQ25792::FieldPosition    BQ25792::WATCHDOG_Position          = 0;

/* [2] */
const BQ25792::FieldMask        BQ25792::FORCE_INDET_Mask           = 0b10000000;
const BQ25792::FieldMask        BQ25792::AUTO_INDET_EN_Mask         = 0b01000000;
const BQ25792::FieldMask        BQ25792::EN_12V_Mask                = 0b00100000;
const BQ25792::FieldMask        BQ25792::EN_9V_Mask                 = 0b00010000;
const BQ25792::FieldMask        BQ25792::HVDCP_EN_Mask              = 0b00001000;
const BQ25792::FieldMask        BQ25792::SDRV_CTRL_Mask             = 0b00000110;
const BQ25792::FieldMask        BQ25792::SDRV_DLY_Mask              = 0b00000001;
const BQ25792::FieldPosition    BQ25792::FORCE_INDET_Position       = 7;
const BQ25792::FieldPosition    BQ25792::AUTO_INDET_EN_Position     = 6;
const BQ25792::FieldPosition    BQ25792::EN_12V_Position            = 5;
const BQ25792::FieldPosition    BQ25792::EN_9V_Position             = 4;
const BQ25792::FieldPosition    BQ25792::HVDCP_EN_Position          = 3;
const BQ25792::FieldPosition    BQ25792::SDRV_CTRL_Position         = 1;
const BQ25792::FieldPosition    BQ25792::SDRV_DLY_Position          = 0;

/* [3] */
const BQ25792::FieldMask        BQ25792::DIS_ACDRV_Mask             = 0b10000000;
const BQ25792::FieldMask        BQ25792::EN_OTG_Mask                = 0b01000000;
const BQ25792::FieldMask        BQ25792::PFM_OTG_DIS_Mask           = 0b00100000;
const BQ25792::FieldMask        BQ25792::PFM_FWD_DIS_Mask           = 0b00010000;
const BQ25792::FieldMask        BQ25792::WKUP_DLY_Mask              = 0b00001000;
const BQ25792::FieldMask        BQ25792::DIS_LDO_Mask               = 0b00000100;
const BQ25792::FieldMask        BQ25792::DIS_OTG_OOA_Mask           = 0b00000010;
const BQ25792::FieldMask        BQ25792::DIS_FWD_OOA_Mask           = 0b00000001;
const BQ25792::FieldPosition    BQ25792::DIS_ACDRV_Position         = 7;
const BQ25792::FieldPosition    BQ25792::EN_OTG_Position            = 6;
const BQ25792::FieldPosition    BQ25792::PFM_OTG_DIS_Position       = 5;
const BQ25792::FieldPosition    BQ25792::PFM_FWD_DIS_Position       = 4;
const BQ25792::FieldPosition    BQ25792::WKUP_DLY_Position          = 3;
const BQ25792::FieldPosition    BQ25792::DIS_LDO_Position           = 2;
const BQ25792::FieldPosition    BQ25792::DIS_OTG_OOA_Position       = 1;
const BQ25792::FieldPosition    BQ25792::DIS_FWD_OOA_Position       = 0;

/* [4] */
const BQ25792::FieldMask        BQ25792::EN_ACDRV2_Mask             = 0b10000000;
const BQ25792::FieldMask        BQ25792::EN_ACDRV1_Mask             = 0b01000000;
const BQ25792::FieldMask        BQ25792::PWM_FREQ_Mask              = 0b00100000;
const BQ25792::FieldMask        BQ25792::DIS_STAT_Mask              = 0b00010000;
const BQ25792::FieldMask        BQ25792::DIS_VSYS_SHORT_Mask        = 0b00001000;
const BQ25792::FieldMask        BQ25792::DIS_VOTG_UVP_Mask          = 0b00000100;
const BQ25792::FieldMask        BQ25792::FORCE_VINDPM_DET_Mask      = 0b00000010;
const BQ25792::FieldMask        BQ25792::EN_IBUS_OCP_Mask           = 0b00000001;
const BQ25792::FieldPosition    BQ25792::EN_ACDRV2_Position         = 7;
const BQ25792::FieldPosition    BQ25792::EN_ACDRV1_Position         = 6;
const BQ25792::FieldPosition    BQ25792::PWM_FREQ_Position          = 5;
const BQ25792::FieldPosition    BQ25792::DIS_STAT_Position          = 4;
const BQ25792::FieldPosition    BQ25792::DIS_VSYS_SHORT_Position    = 3;
const BQ25792::FieldPosition    BQ25792::DIS_VOTG_UVP_Position      = 2;
const BQ25792::FieldPosition    BQ25792::FORCE_VINDPM_DET_Position  = 1;
const BQ25792::FieldPosition    BQ25792::EN_IBUS_OCP_Position       = 0;

/* [5] */
const BQ25792::FieldMask        BQ25792::SFET_PRESENT_Mask          = 0b10000000;
const BQ25792::FieldMask        BQ25792::EN_IBAT_Mask               = 0b00100000;
const BQ25792::FieldMask        BQ25792::IBAT_REG_Mask              = 0b00011000;
const BQ25792::FieldMask        BQ25792::EN_IINDPM_Mask             = 0b00000100;
const BQ25792::FieldMask        BQ25792::EN_EXTILIM_Mask            = 0b00000010;
const BQ25792::FieldMask        BQ25792::EN_BATOC_Mask              = 0b00000001;
const BQ25792::FieldPosition    BQ25792::SFET_PRESENT_Position      = 7;
const BQ25792::FieldPosition    BQ25792::EN_IBAT_Position           = 5;
const BQ25792::FieldPosition    BQ25792::IBAT_REG_Position          = 3;
const BQ25792::FieldPosition    BQ25792::EN_IINDPM_Position         = 2;
const BQ25792::FieldPosition    BQ25792::EN_EXTILIM_Position        = 1;
const BQ25792::FieldPosition    BQ25792::EN_BATOC_Position          = 0;

/* Charger Status */

/* [0] */
const BQ25792::FieldMask        BQ25792::IINDPM_STAT_Mask           = 0b10000000;
const BQ25792::FieldMask        BQ25792::VINDPM_STAT_Mask           = 0b01000000;
const BQ25792::FieldMask        BQ25792::WD_STAT_Mask               = 0b00100000;
const BQ25792::FieldMask        BQ25792::POORSRC_STAT_Mask          = 0b00010000;
const BQ25792::FieldMask        BQ25792::PG_STAT_Mask               = 0b00001000;
const BQ25792::FieldMask        BQ25792::AC2_PRESENT_STAT_Mask      = 0b00000100;
const BQ25792::FieldMask        BQ25792::AC1_PRESENT_STAT_Mask      = 0b00000010;
const BQ25792::FieldMask        BQ25792::VBUS_PRESENT_STAT_Mask     = 0b00000001;
const BQ25792::FieldPosition    BQ25792::IINDPM_STAT_Position       = 7;
const BQ25792::FieldPosition    BQ25792::VINDPM_STAT_Position       = 6;
const BQ25792::FieldPosition    BQ25792::WD_STAT_Position           = 5;
const BQ25792::FieldPosition    BQ25792::POORSRC_STAT_Position      = 4;
const BQ25792::FieldPosition    BQ25792::PG_STAT_Position           = 3;
const BQ25792::FieldPosition    BQ25792::AC2_PRESENT_STAT_Position  = 2;
const BQ25792::FieldPosition    BQ25792::AC1_PRESENT_STAT_Position  = 1;
const BQ25792::FieldPosition    BQ25792::VBUS_PRESENT_STAT_Position = 0;

/* [1] */
const BQ25792::FieldMask        BQ25792::CHG_STAT_Mask              = 0b11100000;
const BQ25792::FieldMask        BQ25792::VBUS_STAT_Mask             = 0b00011110;
const BQ25792::FieldMask        BQ25792::BC12_DONE_STAT_Mask        = 0b00000001;
const BQ25792::FieldPosition    BQ25792::CHG_STAT_Position          = 5;
const BQ25792::FieldPosition    BQ25792::VBUS_STAT_Position         = 1;
const BQ25792::FieldPosition    BQ25792::BC12_DONE_STAT_Position    = 0;

/* [2] */
const BQ25792::FieldMask        BQ25792::ICO_STAT_Mask              = 0b11000000;
const BQ25792::FieldMask        BQ25792::TREG_STAT_Mask             = 0b00000100;
const BQ25792::FieldMask        BQ25792::DPDM_STAT_Mask             = 0b00000010;
const BQ25792::FieldMask        BQ25792::VBAT_PRESENT_STAT_Mask     = 0b00000001;
const BQ25792::FieldPosition    BQ25792::ICO_STAT_Position          = 6;
const BQ25792::FieldPosition    BQ25792::TREG_STAT_Position         = 2;
const BQ25792::FieldPosition    BQ25792::DPDM_STAT_Position         = 1;
const BQ25792::FieldPosition    BQ25792::VBAT_PRESENT_STAT_Position = 0;

/* [3] */
const BQ25792::FieldMask        BQ25792::ACRB2_STAT_Mask            = 0b10000000;
const BQ25792::FieldMask        BQ25792::ACRB1_STAT_Mask            = 0b01000000;
const BQ25792::FieldMask        BQ25792::ADC_DONE_STAT_Mask         = 0b00100000;
const BQ25792::FieldMask        BQ25792::VSYS_STAT_Mask             = 0b00010000;
const BQ25792::FieldMask        BQ25792::CHG_TMR_STAT_Mask          = 0b00001000;
const BQ25792::FieldMask        BQ25792::TRICHG_TMR_STAT_Mask       = 0b00000100;
const BQ25792::FieldMask        BQ25792::PRECHG_TMR_STAT_Mask       = 0b00000010;
const BQ25792::FieldPosition    BQ25792::ACRB2_STAT_Position        = 7;
const BQ25792::FieldPosition    BQ25792::ACRB1_STAT_Position        = 6;
const BQ25792::FieldPosition    BQ25792::ADC_DONE_STAT_Position     = 5;
const BQ25792::FieldPosition    BQ25792::VSYS_STAT_Position         = 4;
const BQ25792::FieldPosition    BQ25792::CHG_TMR_STAT_Position      = 3;
const BQ25792::FieldPosition    BQ25792::TRICHG_TMR_STAT_Position   = 2;
const BQ25792::FieldPosition    BQ25792::PRECHG_TMR_STAT_Position   = 1;

/* [4] */
const BQ25792::FieldMask        BQ25792::VBATOTG_LOW_STAT_Mask      = 0b00010000;
const BQ25792::FieldMask        BQ25792::TS_COLD_STAT_Mask          = 0b00001000;
const BQ25792::FieldMask        BQ25792::TS_COOL_STAT_Mask          = 0b00000100;
const BQ25792::FieldMask        BQ25792::TS_WARM_STAT_Mask          = 0b00000010;
const BQ25792::FieldMask        BQ25792::TS_HOT_STAT_Mask           = 0b00000001;
const BQ25792::FieldPosition    BQ25792::VBATOTG_LOW_STAT_Position  = 4;
const BQ25792::FieldPosition    BQ25792::TS_COLD_STAT_Position      = 3;
const BQ25792::FieldPosition    BQ25792::TS_COOL_STAT_Position      = 2;
const BQ25792::FieldPosition    BQ25792::TS_WARM_STAT_Position      = 1;
const BQ25792::FieldPosition    BQ25792::TS_HOT_STAT_Position       = 0;

/* Charger Flag */

/* [0] */
const BQ25792::FieldMask        BQ25792::IINDPM_FLAG_Mask           = 0b10000000;
const BQ25792::FieldMask        BQ25792::VINDPM_FLAG_Mask           = 0b01000000;
const BQ25792::FieldMask        BQ25792::WD_FLAG_Mask               = 0b00100000;
const BQ25792::FieldMask        BQ25792::POORSRC_FLAG_Mask          = 0b00010000;
const BQ25792::FieldMask        BQ25792::PG_FLAG_Mask               = 0b00001000;
const BQ25792::FieldMask        BQ25792::AC2_PRESENT_FLAG_Mask      = 0b00000100;
const BQ25792::FieldMask        BQ25792::AC1_PRESENT_FLAG_Mask      = 0b00000010;
const BQ25792::FieldMask        BQ25792::VBUS_PRESENT_FLAG_Mask     = 0b00000001;
const BQ25792::FieldPosition    BQ25792::IINDPM_FLAG_Position       = 7;
const BQ25792::FieldPosition    BQ25792::VINDPM_FLAG_Position       = 6;
const BQ25792::FieldPosition    BQ25792::WD_FLAG_Position           = 5;
const BQ25792::FieldPosition    BQ25792::POORSRC_FLAG_Position      = 4;
const BQ25792::FieldPosition    BQ25792::PG_FLAG_Position           = 3;
const BQ25792::FieldPosition    BQ25792::AC2_PRESENT_FLAG_Position  = 2;
const BQ25792::FieldPosition    BQ25792::AC1_PRESENT_FLAG_Position  = 1;
const BQ25792::FieldPosition    BQ25792::VBUS_PRESENT_FLAG_Position = 0;

/* [1] */
const BQ25792::FieldMask        BQ25792::CHG_FLAG_Mask              = 0b10000000;
const BQ25792::FieldMask        BQ25792::ICO_FLAG_Mask              = 0b01000000;
const BQ25792::FieldMask        BQ25792::VBUS_FLAG_Mask             = 0b00010000;
const BQ25792::FieldMask        BQ25792::TREG_FLAG_Mask             = 0b00000100;
const BQ25792::FieldMask        BQ25792::VBAT_PRESENT_FLAG_Mask     = 0b00000010;
const BQ25792::FieldMask        BQ25792::BC12_DONE_FLAG_Mask        = 0b00000001;
const BQ25792::FieldPosition    BQ25792::CHG_FLAG_Position          = 7;
const BQ25792::FieldPosition    BQ25792::ICO_FLAG_Position          = 6;
const BQ25792::FieldPosition    BQ25792::VBUS_FLAG_Position         = 4;
const BQ25792::FieldPosition    BQ25792::TREG_FLAG_Position         = 2;
const BQ25792::FieldPosition    BQ25792::VBAT_PRESENT_FLAG_Position = 1;
const BQ25792::FieldPosition    BQ25792::BC12_DONE_FLAG_Position    = 0;

/* [2] */
const BQ25792::FieldMask        BQ25792::DPDM_DONE_FLAG_Mask        = 0b01000000;
const BQ25792::FieldMask        BQ25792::ADC_DONE_FLAG_Mask         = 0b00100000;
const BQ25792::FieldMask        BQ25792::VSYS_FLAG_Mask             = 0b00010000;
const BQ25792::FieldMask        BQ25792::CHG_TMR_FLAG_Mask          = 0b00001000;
const BQ25792::FieldMask        BQ25792::TRICHG_TMR_FLAG_Mask       = 0b00000100;
const BQ25792::FieldMask        BQ25792::PRECHG_TMR_FLAG_Mask       = 0b00000010;
const BQ25792::FieldMask        BQ25792::TOPOFF_TMR_FLAG_Mask       = 0b00000001;
const BQ25792::FieldPosition    BQ25792::DPDM_DONE_FLAG_Position    = 6;
const BQ25792::FieldPosition    BQ25792::ADC_DONE_FLAG_Position     = 5;
const BQ25792::FieldPosition    BQ25792::VSYS_FLAG_Position         = 4;
const BQ25792::FieldPosition    BQ25792::CHG_TMR_FLAG_Position      = 3;
const BQ25792::FieldPosition    BQ25792::TRICHG_TMR_FLAG_Position   = 2;
const BQ25792::FieldPosition    BQ25792::PRECHG_TMR_FLAG_Position   = 1;
const BQ25792::FieldPosition    BQ25792::TOPOFF_TMR_FLAG_Position   = 0;

/* [3] */
const BQ25792::FieldMask        BQ25792::VBATOTG_LOW_FLAG_Mask      = 0b00010000;
const BQ25792::FieldMask        BQ25792::TS_COLD_FLAG_Mask          = 0b00001000;
const BQ25792::FieldMask        BQ25792::TS_COOL_FLAG_Mask          = 0b00000100;
const BQ25792::FieldMask        BQ25792::TS_WARM_FLAG_Mask          = 0b00000010;
const BQ25792::FieldMask        BQ25792::TS_HOT_FLAG_Mask           = 0b00000001;
const BQ25792::FieldPosition    BQ25792::VBATOTG_LOW_FLAG_Position  = 4;
const BQ25792::FieldPosition    BQ25792::TS_COLD_FLAG_Position      = 3;
const BQ25792::FieldPosition    BQ25792::TS_COOL_FLAG_Position      = 2;
const BQ25792::FieldPosition    BQ25792::TS_WARM_FLAG_Position      = 1;
const BQ25792::FieldPosition    BQ25792::TS_HOT_FLAG_Position       = 0;

/* ADC Control */
const BQ25792::FieldMask        BQ25792::ADC_EN_Mask                = 0b10000000;
const BQ25792::FieldMask        BQ25792::ADC_RATE_Mask              = 0b01000000;
const BQ25792::FieldMask        BQ25792::ADC_SAMPLE_Mask            = 0b00110000;
const BQ25792::FieldMask        BQ25792::ADC_AVG_Mask               = 0b00001000;
const BQ25792::FieldMask        BQ25792::ADC_AVG_INIT_Mask          = 0b00000100;
const BQ25792::FieldPosition    BQ25792::ADC_EN_Position            = 7;
const BQ25792::FieldPosition    BQ25792::ADC_RATE_Position          = 6;
const BQ25792::FieldPosition    BQ25792::ADC_SAMPLE_Position        = 4;
const BQ25792::FieldPosition    BQ25792::ADC_AVG_Position           = 3;
const BQ25792::FieldPosition    BQ25792::ADC_AVG_INIT_Position      = 2;

const double                    BQ25792::TS_ADC_BitStep             = 0.0976563;
const BQ25792::FieldOffset      BQ25792::TS_ADC_Offset              = 0;

const BQ25792::RegDissector BQ25792::regDissector[] = {
    // address                  name                      len  function
    { Minimal_System_Voltage,   "Minimal System Voltage ",  1, dissect_Minimal_System_Voltage },
    { Charge_Voltage_Limit,     "Charge Voltage Limit   ",  2, dissect_Charge_Voltage_Limit },
    { Charge_Current_Limit,     "Charge Current Limit   ",  2, dissect_Charge_Current_Limit },
    { Input_Voltage_Limit,      "Input Voltage Limit    ",  1, dissect_Input_Voltage_Limit },
    { Input_Current_Limit,      "Input Current Limit    ",  2, dissect_Input_Current_Limit },
    { Precharge_Control,        "Precharge Control      ",  1, nullptr },
    { Termination_Control,      "Termination Control    ",  1, dissect_Termination_Control },
    { Recharge_Control,         "Re-charge Control      ",  1, nullptr },
    { VOTG_regulation,          "VOTG regulation        ",  2, nullptr },
    { IOTG_regulation,          "IOTG regulation        ",  1, nullptr },
    { Timer_Control,            "Timer Control          ",  1, nullptr },
    { Charger_Control,          "Charger Control        ",  6, dissect_Charger_Control },
    { Temperature_Control,      "Temperature Control    ",  1, nullptr },
    { NTC_Control,              "NTC Control            ",  2, nullptr },
    { ICO_Current_Limit,        "ICO Current Limit      ",  2, nullptr },
    { Charger_Status,           "Charger Status         ",  5, dissect_Charger_Status },
    { FAULT_Status,             "FAULT Status           ",  2, nullptr },
    { Charger_Flag,             "Charger Flag           ",  4, dissect_Charger_Flag },
    { FAULT_Flag,               "FAULT Flag             ",  2, nullptr },
    { Charger_Mask,             "Charger Mask           ",  4, nullptr },
    { FAULT_Mask,               "FAULT Mask             ",  2, nullptr },
    { ADC_Control,              "ADC Control            ",  1, dissect_ADC_Control },
    { ADC_Function_Disable,     "ADC Function Disable   ",  2, nullptr },
    { IBUS_ADC,                 "IBUS ADC               ",  2, dissect_IBUS_ADC },
    { IBAT_ADC,                 "IBAT ADC               ",  2, dissect_IBAT_ADC },
    { VBUS_ADC,                 "VBUS ADC               ",  2, dissect_VBUS_ADC },
    { VAC1_ADC,                 "VAC1 ADC               ",  2, dissect_VAC1_ADC },
    { VAC2_ADC,                 "VAC2 ADC               ",  2, dissect_VAC2_ADC },
    { VBAT_ADC,                 "VBAT ADC               ",  2, dissect_VBAT_ADC },
    { VSYS_ADC,                 "VSYS ADC               ",  2, dissect_VSYS_ADC },
    { TS_ADC,                   "TS ADC                 ",  2, dissect_TS_ADC },
    { TDIE_ADC,                 "TDIE ADC               ",  2, nullptr },
    { DP_ADC,                   "D+ ADC                 ",  2, nullptr },
    { DN_ADC,                   "D- ADC                 ",  2, nullptr },
    { DPDM_Driver,              "DPDM Driver            ",  1, nullptr },
    { Part_Information,         "Part Information       ",  1, nullptr }
};

const byte       BQ25792::I2C_ADDRESS       = 0x6B;
const byte       BQ25792::I2C_DATA_SIZE_MAX = 6;
const byte       BQ25792::REG_SIZE          = sizeof(regDissector) / sizeof(RegDissector);

/*****************************************************************************/

void 
BQ25792::dissect_Minimal_System_Voltage(byte data[], byte size)
{
    double VSYSMIN = (((double) (data[0] & VSYSMIN_Mask)) * VSYSMIN_BitStep + VSYSMIN_Offset) / mV_to_V;
    Serial.print("  VSYSMIN                 = ");
    Serial.println(VSYSMIN);
}
void
BQ25792::dissect_Charge_Voltage_Limit(byte data[], byte size)
{
    double VREG = (((double) (((data[0] & VREG_Mask) << 8) | data[1])) * VREG_BitStep + VREG_Offset) / mV_to_V;
    Serial.print("  VREG                    = ");
    Serial.println(VREG);
}

void
BQ25792::dissect_Charge_Current_Limit(byte data[], byte size)
{
    double ICHG = (((double) (((data[0] & ICHG_Mask) << 8) | data[1])) * ICHG_BitStep + ICHG_Offset) / mV_to_V;
    Serial.print("  ICHG                    = ");
    Serial.println(ICHG);
}

void
BQ25792::dissect_Input_Voltage_Limit(byte data[], byte size)
{
    double VINDPM = (((double) (data[0])) * VINDPM_BitStep + VINDPM_Offset) / mV_to_V;
    Serial.print("  VINDPM                  = ");
    Serial.println(VINDPM);
}

void 
BQ25792::dissect_Input_Current_Limit(byte data[], byte size)
{
    double IINDPM = (((double) (((data[0] & IINDPM_Mask) << 8) | data[1])) * IINDPM_BitStep + IINDPM_Offset) / mV_to_V;
    Serial.print("  IINDPM                  = ");
    Serial.println(IINDPM);
}

void 
BQ25792::dissect_Termination_Control(byte data[], byte size)
{
    byte REG_RST = ((data[0] & REG_RST_Mask) >> REG_RST_Position);
    byte ITERM   = ((double) ((data[0] & ITERM_Mask) >> ITERM_Position) * ITERM_BitStep + ITERM_Offset) / mV_to_V;

    Serial.print("  REG_RST                 = ");
    Serial.println(REG_RST);
    
    Serial.print("  ITERM                   = ");
    Serial.println(ITERM);
}

void
BQ25792::dissect_Charger_Control(byte data[], byte size)
{
    /* data[0] */
    byte EN_AUTO_IBATDIS    = ((data[0] & EN_AUTO_IBATDIS_Mask) >> EN_AUTO_IBATDIS_Position);
    byte FORCE_IBATDIS      = ((data[0] & FORCE_IBATDIS_Mask)   >> FORCE_IBATDIS_Position);
    byte EN_CHG             = ((data[0] & EN_CHG_Mask)          >> EN_CHG_Position);
    byte EN_ICO             = ((data[0] & EN_ICO_Mask)          >> EN_ICO_Position);
    byte FORCE_ICO          = ((data[0] & FORCE_ICO_Mask)       >> FORCE_ICO_Position);
    byte EN_HIZ             = ((data[0] & EN_HIZ_Mask)          >> EN_HIZ_Position);
    byte EN_TERM            = ((data[0] & EN_TERM_Mask)         >> EN_TERM_Position);

    Serial.println("  [0]");

    Serial.print("    EN_AUTO_IBATDIS       = ");
    Serial.println(EN_AUTO_IBATDIS);
    
    Serial.print("    FORCE_IBATDIS         = ");
    Serial.println(FORCE_IBATDIS);
    
    Serial.print("    EN_CHG                = ");
    Serial.println(EN_CHG);
    
    Serial.print("    EN_ICO                = ");
    Serial.println(EN_ICO);
    
    Serial.print("    FORCE_ICO             = ");
    Serial.println(FORCE_ICO);
    
    Serial.print("    EN_HIZ                = ");
    Serial.println(EN_HIZ);
    
    Serial.print("    EN_TERM               = ");
    Serial.println(EN_TERM);

    /* data[1] */
    byte VAC_OVP            = ((data[1] & VAC_OVP_Mask)     >> VAC_OVP_Position);
    byte WD_RST             = ((data[1] & WD_RST_Mask)      >> WD_RST_Position);
    byte WATCHDOG           = ((data[1] & WATCHDOG_Mask)    >> WATCHDOG_Position);

    Serial.println("  [1]");
    
    Serial.print("    VAC_OVP               = ");
    Serial.println(VAC_OVP);
    
    Serial.print("    WD_RST                = ");
    Serial.println(WD_RST);
    
    Serial.print("    WATCHDOG              = ");
    Serial.println(WATCHDOG);

    /* data[2] */
    byte FORCE_INDET        = ((data[2] & FORCE_INDET_Mask)     >> FORCE_INDET_Position);
    byte AUTO_INDET_EN      = ((data[2] & AUTO_INDET_EN_Mask)   >> AUTO_INDET_EN_Position);
    byte EN_12V             = ((data[2] & EN_12V_Mask)          >> EN_12V_Position);
    byte EN_9V              = ((data[2] & EN_9V_Mask)           >> EN_9V_Position);
    byte HVDCP_EN           = ((data[2] & HVDCP_EN_Mask)        >> HVDCP_EN_Position);
    byte SDRV_CTRL          = ((data[2] & SDRV_CTRL_Mask)       >> SDRV_CTRL_Position);
    byte SDRV_DLY           = ((data[2] & SDRV_DLY_Mask)        >> SDRV_DLY_Position);
    
    Serial.println("  [2]");

    Serial.print("    FORCE_INDET           = ");
    Serial.println(FORCE_INDET);
    
    Serial.print("    AUTO_INDET_EN         = ");
    Serial.println(AUTO_INDET_EN);
    
    Serial.print("    EN_12V                = ");
    Serial.println(EN_12V);
    
    Serial.print("    EN_9V                 = ");
    Serial.println(EN_9V);
    
    Serial.print("    HVDCP_EN              = ");
    Serial.println(HVDCP_EN);
    
    Serial.print("    SDRV_CTRL             = ");
    Serial.println(SDRV_CTRL);
    
    Serial.print("    SDRV_DLY              = ");
    Serial.println(SDRV_DLY);

    /* data[3] */
    byte DIS_ACDRV           = ((data[3] & DIS_ACDRV_Mask)      >> DIS_ACDRV_Position);
    byte EN_OTG              = ((data[3] & EN_OTG_Mask)         >> EN_OTG_Position);
    byte PFM_OTG_DIS         = ((data[3] & PFM_OTG_DIS_Mask)    >> PFM_OTG_DIS_Position);
    byte PFM_FWD_DIS         = ((data[3] & PFM_FWD_DIS_Mask)    >> PFM_FWD_DIS_Position);
    byte WKUP_DLY            = ((data[3] & WKUP_DLY_Mask)       >> WKUP_DLY_Position);
    byte DIS_LDO             = ((data[3] & DIS_LDO_Mask)        >> DIS_LDO_Position);
    byte DIS_OTG_OOA         = ((data[3] & DIS_OTG_OOA_Mask)    >> DIS_OTG_OOA_Position);
    byte DIS_FWD_OOA         = ((data[3] & DIS_FWD_OOA_Mask)    >> DIS_FWD_OOA_Position);
    
    Serial.println("  [3]");

    Serial.print("    DIS_ACDRV             = ");
    Serial.println(DIS_ACDRV);
    
    Serial.print("    EN_OTG                = ");
    Serial.println(EN_OTG);
    
    Serial.print("    PFM_OTG_DIS           = ");
    Serial.println(PFM_OTG_DIS);
    
    Serial.print("    PFM_FWD_DIS           = ");
    Serial.println(PFM_FWD_DIS);
    
    Serial.print("    WKUP_DLY              = ");
    Serial.println(WKUP_DLY);
    
    Serial.print("    DIS_LDO               = ");
    Serial.println(DIS_LDO);
    
    Serial.print("    DIS_OTG_OOA           = ");
    Serial.println(DIS_OTG_OOA);
    
    Serial.print("    DIS_FWD_OOA           = ");
    Serial.println(DIS_FWD_OOA);

    /* data[4] */
    byte EN_ACDRV2           = ((data[4] & EN_ACDRV2_Mask)          >> EN_ACDRV2_Position);
    byte EN_ACDRV1           = ((data[4] & EN_ACDRV1_Mask)          >> EN_ACDRV1_Position);
    byte PWM_FREQ            = ((data[4] & PWM_FREQ_Mask)           >> PWM_FREQ_Position);
    byte DIS_STAT            = ((data[4] & DIS_STAT_Mask)           >> DIS_STAT_Position);
    byte DIS_VSYS_SHORT      = ((data[4] & DIS_VSYS_SHORT_Mask)     >> DIS_VSYS_SHORT_Position);
    byte DIS_VOTG_UVP        = ((data[4] & DIS_VOTG_UVP_Mask)       >> DIS_VOTG_UVP_Position);
    byte FORCE_VINDPM_DET    = ((data[4] & FORCE_VINDPM_DET_Mask)   >> FORCE_VINDPM_DET_Position);
    byte EN_IBUS_OCP         = ((data[4] & EN_IBUS_OCP_Mask)        >> EN_IBUS_OCP_Position);
    
    Serial.println("  [4]");

    Serial.print("    EN_ACDRV2             = ");
    Serial.println(EN_ACDRV2);
    
    Serial.print("    EN_ACDRV1             = ");
    Serial.println(EN_ACDRV1);
    
    Serial.print("    PWM_FREQ              = ");
    Serial.println(PWM_FREQ);
    
    Serial.print("    DIS_STAT              = ");
    Serial.println(DIS_STAT);
    
    Serial.print("    DIS_VSYS_SHORT        = ");
    Serial.println(DIS_VSYS_SHORT);
    
    Serial.print("    DIS_VOTG_UVP          = ");
    Serial.println(DIS_VOTG_UVP);
    
    Serial.print("    FORCE_VINDPM_DET      = ");
    Serial.println(FORCE_VINDPM_DET);
    
    Serial.print("    EN_IBUS_OCP           = ");
    Serial.println(EN_IBUS_OCP);

    /* data[5] */
    byte SFET_PRESENT       = ((data[5] & SFET_PRESENT_Mask)    >> SFET_PRESENT_Position);
    byte EN_IBAT            = ((data[5] & EN_IBAT_Mask)         >> EN_IBAT_Position);
    byte IBAT_REG           = ((data[5] & IBAT_REG_Mask)        >> IBAT_REG_Position);
    byte EN_IINDPM          = ((data[5] & EN_IINDPM_Mask)       >> EN_IINDPM_Position);
    byte EN_EXTILIM         = ((data[5] & EN_EXTILIM_Mask)      >> EN_EXTILIM_Position);
    byte EN_BATOC           = ((data[5] & EN_BATOC_Mask)        >> EN_BATOC_Position);
    
    Serial.println("  [5]");

    Serial.print("    SFET_PRESENT          = ");
    Serial.println(SFET_PRESENT);
    
    Serial.print("    EN_IBAT               = ");
    Serial.println(EN_IBAT);
    
    Serial.print("    IBAT_REG              = ");
    Serial.println(IBAT_REG);
    
    Serial.print("    EN_IINDPM             = ");
    Serial.println(EN_IINDPM);
    
    Serial.print("    EN_EXTILIM            = ");
    Serial.println(EN_EXTILIM);
    
    Serial.print("    EN_BATOC              = ");
    Serial.println(EN_BATOC);
}

void
BQ25792::dissect_Charger_Status(byte data[], byte size)
{

    /* data[0] */
    byte IINDPM_STAT        = ((data[0] & IINDPM_STAT_Mask)         >> IINDPM_STAT_Position);
    byte VINDPM_STAT        = ((data[0] & VINDPM_STAT_Mask)         >> VINDPM_STAT_Position);
    byte WD_STAT            = ((data[0] & WD_STAT_Mask)             >> WD_STAT_Position);
    byte POORSRC_STAT       = ((data[0] & POORSRC_STAT_Mask)        >> POORSRC_STAT_Position);
    byte PG_STAT            = ((data[0] & PG_STAT_Mask)             >> PG_STAT_Position);
    byte AC2_PRESENT_STAT   = ((data[0] & AC2_PRESENT_STAT_Mask)    >> AC2_PRESENT_STAT_Position);
    byte AC1_PRESENT_STAT   = ((data[0] & AC1_PRESENT_STAT_Mask)    >> AC1_PRESENT_STAT_Position);
    byte VBUS_PRESENT_STAT  = ((data[0] & VBUS_PRESENT_STAT_Mask)   >> VBUS_PRESENT_STAT_Position);

    Serial.println("  [0]");

    Serial.print("    IINDPM_STAT           = ");
    Serial.println(IINDPM_STAT);
    
    Serial.print("    VINDPM_STAT           = ");
    Serial.println(VINDPM_STAT);
    
    Serial.print("    WD_STAT               = ");
    Serial.println(WD_STAT);
    
    Serial.print("    POORSRC_STAT          = ");
    Serial.println(POORSRC_STAT);
    
    Serial.print("    PG_STAT               = ");
    Serial.println(PG_STAT);
    
    Serial.print("    AC2_PRESENT_STAT      = ");
    Serial.println(AC2_PRESENT_STAT);
    
    Serial.print("    AC1_PRESENT_STAT      = ");
    Serial.println(AC1_PRESENT_STAT);
    
    Serial.print("    VBUS_PRESENT_STAT     = ");
    Serial.println(VBUS_PRESENT_STAT);
    
    /* data[1] */
    byte CHG_STAT           = ((data[1] & CHG_STAT_Mask)            >> CHG_STAT_Position);
    byte VBUS_STAT          = ((data[1] & VBUS_STAT_Mask)           >> VBUS_STAT_Position);
    byte BC12_DONE_STAT     = ((data[1] & BC12_DONE_STAT_Mask)      >> BC12_DONE_STAT_Position);

    Serial.println("  [1]");

    Serial.print("    CHG_STAT              = ");
    Serial.println(CHG_STAT);
    
    Serial.print("    VBUS_STAT             = ");
    Serial.println(VBUS_STAT);
    
    Serial.print("    BC12_DONE_STAT        = ");
    Serial.println(BC12_DONE_STAT);
    
    /* data[2] */
    byte ICO_STAT           = ((data[2] & ICO_STAT_Mask)            >> ICO_STAT_Position);
    byte TREG_STAT          = ((data[2] & TREG_STAT_Mask)           >> TREG_STAT_Position);
    byte DPDM_STAT          = ((data[2] & DPDM_STAT_Mask)           >> DPDM_STAT_Position);
    byte VBAT_PRESENT_STAT  = ((data[2] & VBAT_PRESENT_STAT_Mask)   >> VBAT_PRESENT_STAT_Position);

    Serial.println("  [2]");

    Serial.print("    ICO_STAT              = ");
    Serial.println(ICO_STAT);
    
    Serial.print("    TREG_STAT             = ");
    Serial.println(TREG_STAT);
    
    Serial.print("    DPDM_STAT             = ");
    Serial.println(DPDM_STAT);
    
    Serial.print("    VBAT_PRESENT_STAT     = ");
    Serial.println(VBAT_PRESENT_STAT);

    /* data[3] */
    byte ACRB2_STAT         = ((data[3] & ACRB2_STAT_Mask)          >> ACRB2_STAT_Position);
    byte ACRB1_STAT         = ((data[3] & ACRB1_STAT_Mask)          >> ACRB1_STAT_Position);
    byte ADC_DONE_STAT      = ((data[3] & ADC_DONE_STAT_Mask)       >> ADC_DONE_STAT_Position);
    byte VSYS_STAT          = ((data[3] & VSYS_STAT_Mask)           >> VSYS_STAT_Position);
    byte CHG_TMR_STAT       = ((data[3] & CHG_TMR_STAT_Mask)        >> CHG_TMR_STAT_Position);
    byte TRICHG_TMR_STAT    = ((data[3] & TRICHG_TMR_STAT_Mask)     >> TRICHG_TMR_STAT_Position);
    byte PRECHG_TMR_STAT    = ((data[3] & PRECHG_TMR_STAT_Mask)     >> PRECHG_TMR_STAT_Position);

    Serial.println("  [3]");

    Serial.print("    ACRB2_STAT            = ");
    Serial.println(ACRB2_STAT);
    
    Serial.print("    ACRB1_STAT            = ");
    Serial.println(ACRB1_STAT);
    
    Serial.print("    ADC_DONE_STAT         = ");
    Serial.println(ADC_DONE_STAT);
    
    Serial.print("    VSYS_STAT             = ");
    Serial.println(VSYS_STAT);
    
    Serial.print("    CHG_TMR_STAT          = ");
    Serial.println(CHG_TMR_STAT);
    
    Serial.print("    TRICHG_TMR_STAT       = ");
    Serial.println(TRICHG_TMR_STAT);
    
    Serial.print("    PRECHG_TMR_STAT       = ");
    Serial.println(PRECHG_TMR_STAT);

    /* data[4] */
    byte VBATOTG_LOW_STAT   = ((data[3] & VBATOTG_LOW_STAT_Mask)    >> VBATOTG_LOW_STAT_Position);
    byte TS_COLD_STAT       = ((data[3] & TS_COLD_STAT_Mask)        >> TS_COLD_STAT_Position);
    byte TS_COOL_STAT       = ((data[3] & TS_COOL_STAT_Mask)        >> TS_COOL_STAT_Position);
    byte TS_WARM_STAT       = ((data[3] & TS_WARM_STAT_Mask)        >> TS_WARM_STAT_Position);
    byte TS_HOT_STAT        = ((data[3] & TS_HOT_STAT_Mask)         >> TS_HOT_STAT_Position);

    Serial.println("  [3]");

    Serial.print("    VBATOTG_LOW_STAT      = ");
    Serial.println(VBATOTG_LOW_STAT);
    
    Serial.print("    TS_COLD_STAT          = ");
    Serial.println(TS_COLD_STAT);
    
    Serial.print("    TS_COOL_STAT          = ");
    Serial.println(TS_COOL_STAT);
    
    Serial.print("    TS_WARM_STAT          = ");
    Serial.println(TS_WARM_STAT);
    
    Serial.print("    TS_HOT_STAT           = ");
    Serial.println(TS_HOT_STAT);
}

void
BQ25792::dissect_Charger_Flag(byte data[], byte size)
{
    /* data[0] */
    byte IINDPM_FLAG        = ((data[0] & IINDPM_FLAG_Mask)         >> IINDPM_FLAG_Position);
    byte VINDPM_FLAG        = ((data[0] & VINDPM_FLAG_Mask)         >> VINDPM_FLAG_Position);
    byte WD_FLAG            = ((data[0] & WD_FLAG_Mask)             >> WD_FLAG_Position);
    byte POORSRC_FLAG       = ((data[0] & POORSRC_FLAG_Mask)        >> POORSRC_FLAG_Position);
    byte PG_FLAG            = ((data[0] & PG_FLAG_Mask)             >> PG_FLAG_Position);
    byte AC2_PRESENT_FLAG   = ((data[0] & AC2_PRESENT_FLAG_Mask)    >> AC2_PRESENT_FLAG_Position);
    byte AC1_PRESENT_FLAG   = ((data[0] & AC1_PRESENT_FLAG_Mask)    >> AC1_PRESENT_FLAG_Position);
    byte VBUS_PRESENT_FLAG  = ((data[0] & VBUS_PRESENT_FLAG_Mask)   >> VBUS_PRESENT_FLAG_Position);

    Serial.println("  [0]");

    Serial.print("    IINDPM_FLAG           = ");
    Serial.println(IINDPM_FLAG);
    
    Serial.print("    VINDPM_FLAG           = ");
    Serial.println(VINDPM_FLAG);
    
    Serial.print("    WD_FLAG               = ");
    Serial.println(WD_FLAG);
    
    Serial.print("    POORSRC_FLAG          = ");
    Serial.println(POORSRC_FLAG);
    
    Serial.print("    PG_FLAG               = ");
    Serial.println(PG_FLAG);
    
    Serial.print("    AC2_PRESENT_FLAG      = ");
    Serial.println(AC2_PRESENT_FLAG);
    
    Serial.print("    AC1_PRESENT_FLAG      = ");
    Serial.println(AC1_PRESENT_FLAG);
    
    Serial.print("    VBUS_PRESENT_FLAG     = ");
    Serial.println(VBUS_PRESENT_FLAG);

    /* data[1] */
    byte CHG_FLAG           = ((data[1] & CHG_FLAG_Mask)            >> CHG_FLAG_Position);
    byte ICO_FLAG           = ((data[1] & ICO_FLAG_Mask)            >> ICO_FLAG_Position);
    byte VBUS_FLAG          = ((data[1] & VBUS_FLAG_Mask)           >> VBUS_FLAG_Position);
    byte TREG_FLAG          = ((data[1] & TREG_FLAG_Mask)           >> TREG_FLAG_Position);
    byte VBAT_PRESENT_FLAG  = ((data[1] & VBAT_PRESENT_FLAG_Mask)   >> VBAT_PRESENT_FLAG_Position);
    byte BC12_DONE_FLAG     = ((data[1] & BC12_DONE_FLAG_Mask)      >> BC12_DONE_FLAG_Position);

    Serial.println("  [1]");

    Serial.print("    CHG_FLAG              = ");
    Serial.println(CHG_FLAG);

    Serial.print("    ICO_FLAG              = ");
    Serial.println(ICO_FLAG);

    Serial.print("    VBUS_FLAG             = ");
    Serial.println(VBUS_FLAG);

    Serial.print("    TREG_FLAG             = ");
    Serial.println(TREG_FLAG);

    Serial.print("    VBAT_PRESENT_FLAG     = ");
    Serial.println(VBAT_PRESENT_FLAG);

    Serial.print("    BC12_DONE_FLAG        = ");
    Serial.println(BC12_DONE_FLAG);

    /* data[2] */
    byte DPDM_DONE_FLAG     = ((data[1] & DPDM_DONE_FLAG_Mask)      >> DPDM_DONE_FLAG_Position);
    byte ADC_DONE_FLAG      = ((data[1] & ADC_DONE_FLAG_Mask)       >> ADC_DONE_FLAG_Position);
    byte VSYS_FLAG          = ((data[1] & VSYS_FLAG_Mask)           >> VSYS_FLAG_Position);
    byte CHG_TMR_FLAG       = ((data[1] & CHG_TMR_FLAG_Mask)        >> CHG_TMR_FLAG_Position);
    byte TRICHG_TMR_FLAG    = ((data[1] & TRICHG_TMR_FLAG_Mask)     >> TRICHG_TMR_FLAG_Position);
    byte PRECHG_TMR_FLAG    = ((data[1] & PRECHG_TMR_FLAG_Mask)     >> PRECHG_TMR_FLAG_Position);
    byte TOPOFF_TMR_FLAG    = ((data[1] & TOPOFF_TMR_FLAG_Mask)     >> TOPOFF_TMR_FLAG_Position);

    Serial.println("  [2]");

    Serial.print("    DPDM_DONE_FLAG        = ");
    Serial.println(DPDM_DONE_FLAG);

    Serial.print("    ADC_DONE_FLAG         = ");
    Serial.println(ADC_DONE_FLAG);

    Serial.print("    VSYS_FLAG             = ");
    Serial.println(VSYS_FLAG);

    Serial.print("    CHG_TMR_FLAG          = ");
    Serial.println(CHG_TMR_FLAG);

    Serial.print("    TRICHG_TMR_FLAG       = ");
    Serial.println(TRICHG_TMR_FLAG);

    Serial.print("    PRECHG_TMR_FLAG       = ");
    Serial.println(PRECHG_TMR_FLAG);

    Serial.print("    TOPOFF_TMR_FLAG       = ");
    Serial.println(TOPOFF_TMR_FLAG);
    
    /* data[3] */
    byte VBATOTG_LOW_FLAG   = ((data[1] & VBATOTG_LOW_FLAG_Mask)    >> VBATOTG_LOW_FLAG_Position);
    byte TS_COLD_FLAG       = ((data[1] & TS_COLD_FLAG_Mask)        >> TS_COLD_FLAG_Position);
    byte TS_COOL_FLAG       = ((data[1] & TS_COOL_FLAG_Mask)        >> TS_COOL_FLAG_Position);
    byte TS_WARM_FLAG       = ((data[1] & TS_WARM_FLAG_Mask)        >> TS_WARM_FLAG_Position);
    byte TS_HOT_FLAG        = ((data[1] & TS_HOT_FLAG_Mask)         >> TS_HOT_FLAG_Position);

    Serial.println("  [3]");

    Serial.print("    VBATOTG_LOW_FLAG      = ");
    Serial.println(VBATOTG_LOW_FLAG);

    Serial.print("    TS_COLD_FLAG          = ");
    Serial.println(TS_COLD_FLAG);

    Serial.print("    TS_COOL_FLAG          = ");
    Serial.println(TS_COOL_FLAG);

    Serial.print("    TS_WARM_FLAG          = ");
    Serial.println(TS_WARM_FLAG);

    Serial.print("    TS_HOT_FLAG           = ");
    Serial.println(TS_HOT_FLAG);
}

void
BQ25792::dissect_ADC_Control(byte data[], byte size)
{
    /* data[0] */
    byte ADC_EN             = ((data[0] & ADC_EN_Mask)          >> ADC_EN_Position);
    byte ADC_RATE           = ((data[0] & ADC_RATE_Mask)        >> ADC_RATE_Position);
    byte ADC_SAMPLE         = ((data[0] & ADC_SAMPLE_Mask)      >> ADC_SAMPLE_Position);
    byte ADC_AVG            = ((data[0] & ADC_AVG_Mask)         >> ADC_AVG_Position);
    byte ADC_AVG_INIT       = ((data[0] & ADC_AVG_INIT_Mask)    >> ADC_AVG_INIT_Position);

    Serial.print("  ADC_EN                  = ");
    Serial.println(ADC_EN);
    
    Serial.print("  ADC_RATE                = ");
    Serial.println(ADC_RATE);
    
    Serial.print("  ADC_SAMPLE              = ");
    Serial.println(ADC_SAMPLE);
    
    Serial.print("  ADC_AVG                 = ");
    Serial.println(ADC_AVG);
    
    Serial.print("  ADC_AVG_INIT            = ");
    Serial.println(ADC_AVG_INIT);
}

void
BQ25792::dissect_IBUS_ADC(byte data[], byte size)
{
    double IBUS_ADC = ((double) ((int16_t) ((data[0] << 8) | data[1]))) / mV_to_V;
    Serial.print("  IBUS ADC                = ");
    Serial.println(IBUS_ADC);
}

void
BQ25792::dissect_IBAT_ADC(byte data[], byte size)
{
    double IBAT_ADC = ((double) ((int16_t) ((data[0] << 8) | data[1]))) / mV_to_V;
    Serial.print("  IBAT ADC                = ");
    Serial.println(IBAT_ADC);
}

void
BQ25792::dissect_VBUS_ADC(byte data[], byte size)
{
    double VBUS_ADC = ((double) ((data[0] << 8) | data[1])) / mV_to_V;
    Serial.print("  VBUS ADC                = ");
    Serial.println(VBUS_ADC);
}

void
BQ25792::dissect_VAC1_ADC(byte data[], byte size)
{
    double VAC1_ADC = ((double) ((data[0] << 8) | data[1])) / mV_to_V;
    Serial.print("  VAC1 ADC                = ");
    Serial.println(VAC1_ADC);
}

void
BQ25792::dissect_VAC2_ADC(byte data[], byte size)
{
    double VAC2_ADC = ((double) ((data[0] << 8) | data[1])) / mV_to_V;
    Serial.print("  VAC2 ADC                = ");
    Serial.println(VAC2_ADC);
}

void
BQ25792::dissect_VBAT_ADC(byte data[], byte size)
{
    double VBAT_ADC = ((double) ((data[0] << 8) | data[1])) / mV_to_V;
    Serial.print("  VBAT ADC                = ");
    Serial.println(VBAT_ADC);
}

void
BQ25792::dissect_VSYS_ADC(byte data[], byte size)
{
    double VSYS_ADC = ((double) ((data[0] << 8) | data[1])) / mV_to_V;
    Serial.print("  VSYS ADC                = ");
    Serial.println(VSYS_ADC);
}

void
BQ25792::dissect_TS_ADC(byte data[], byte size)
{
    double TS_ADC = (((double) ((data[0] << 8) | data[1])) * TS_ADC_BitStep + TS_ADC_Offset) / mV_to_V;
    Serial.print("  TS ADC                  = ");
    Serial.print(TS_ADC);
    Serial.println("%");
}


/*****************************************************************************/

void
BQ25792::begin()
{
    WATCHDOG(0x00);
    ADC_EN(0x01);
    EN_IBAT(0x01);
    EN_EXTILIM(0x00); // disable ILIM_HIZ
    SFET_PRESENT(0x01);
    SDRV_DLY(0x01);
    WKUP_DLY(0x01);
}

/*****************************************************************************/

/*
  [0]
    EN_AUTO_IBATDIS       = 1
    FORCE_IBATDIS         = 0
    EN_CHG                = 1
    EN_ICO                = 0
    FORCE_ICO             = 0
    EN_HIZ                = 0
    EN_TERM               = 1
  [1]
    VAC_OVP               = 0
    WD_RST                = 0
    WATCHDOG              = 5
  [2]
    FORCE_INDET           = 0
    AUTO_INDET_EN         = 1
    EN_12V                = 0
    EN_9V                 = 0
    HVDCP_EN              = 0
    SDRV_CTRL             = 0
    SDRV_DLY              = 1
  [3]
    DIS_ACDRV             = 0
    EN_OTG                = 0
    PFM_OTG_DIS           = 0
    PFM_FWD_DIS           = 0
    WKUP_DLY              = 0
    DIS_LDO               = 0
    DIS_OTG_OOA           = 0
    DIS_FWD_OOA           = 0
  [4]
    EN_ACDRV2             = 0
    EN_ACDRV1             = 0
    PWM_FREQ              = 0
    DIS_STAT              = 0
    DIS_VSYS_SHORT        = 0
    DIS_VOTG_UVP          = 0
    FORCE_VINDPM_DET      = 0
    EN_IBUS_OCP           = 1
  [5]
    SFET_PRESENT          = 1
    EN_IBAT               = 0
    IBAT_REG              = 2
    EN_IINDPM             = 1
    EN_EXTILIM            = 1
    EN_BATOC              = 0
*/

void
BQ25792::FIELD_bit(byte reg, byte mask, byte offset, byte data)
{
    byte len = Wire.requestFrom(I2C_ADDRESS, 1, reg, 1, true);
    byte req;
    byte resp;
    if (len > 0) {
        req = Wire.read();
        resp = (req & ~mask) | (data << offset);

        printHex(reg);
        Serial.print(": ");
        //printHex(req);
        printBinary(req);
        Serial.print(" & ");
        printBinary(mask);
        Serial.print(" (");
        printBinary(req & ~mask);
        Serial.print(") => ");
        //printHex(resp);
        printBinary(resp);
        Serial.println("");

        sendBytes(I2C_ADDRESS, reg, &resp, 1);
    }
}

void
BQ25792::FIELD_word(byte reg, uint16_t mask, uint16_t offset, uint16_t data)
{
    const byte size = 2;
    byte k;
    byte len = Wire.requestFrom(I2C_ADDRESS, size, reg, 1, true);
    byte swap;
    union {
        uint16_t req_16bit;
        byte     req_byte[size];
    } req;
    
    union {
        uint16_t resp_16bit;
        byte     resp_byte[size];
    } resp;

    //byte reset1 = (reset >> 0) & 0xff;
    //byte reset2 = (reset >> 8) & 0xff;
    if (len > 0) {
        
        printHex(reg);
        Serial.print(": ");
        for (k = 0; Wire.available() && k < size; k++) {
            req.req_byte[k] = Wire.read();
            printHex(req.req_byte[k]);
            Serial.print(" ");
        }

        swap = resp.resp_byte[0];
        resp.resp_byte[0] = resp.resp_byte[1];
        resp.resp_byte[1] = swap;

        resp.resp_16bit = (req.req_16bit & ~mask) | (data << offset);

        //printHex(req);
        printBinary(req.req_byte[0]);
        Serial.print(" ");
        printBinary(req.req_byte[1]);
        Serial.print(" => ");
        //printHex(resp);
        printBinary(resp.resp_byte[0]);
        Serial.print(" ");
        printBinary(resp.resp_byte[1]);
        Serial.println("");

        sendBytes(I2C_ADDRESS, reg, resp.resp_byte, size);
    }
}


/* reg=0x06 name=Input Current Limit *****************************************/

void
BQ25792::IINDPM(uint16_t data)
{
    FIELD_word(Input_Current_Limit, IINDPM_Mask, 0, data);
}

/* reg=0x09 name=Termination Control *****************************************/

void
BQ25792::REG_RST(byte data)
{
    FIELD_bit(Termination_Control, REG_RST_Mask, REG_RST_Position, data);
}

/* reg=0x0F name=Charger Control *********************************************/


/*=== [1] ===*/

/**
 * I2C watch dog timer reset
 * 0 = Normal (default)
 * 1 = Reset (this bit goes back to 0 after timer resets)
 */
void
BQ25792::WD_RST(byte data)
{
    FIELD_bit(Charger_Control + 1, WD_RST_Mask, WD_RST_Position, data);
}

/**
 * Watchdog timer settings
 * 0 = Disable
 * 1 = 0.5s
 * 2 = 1s
 * 3 = 2s
 * 4 = 20s
 * 5 = 40s (default)
 * 6 = 80s
 * 7 = 160s
 */
void
BQ25792::WATCHDOG(byte data)
{
    FIELD_bit(Charger_Control + 1, WATCHDOG_Mask, WATCHDOG_Position, data);
}

/*=== [2] ===*/

/**
 * High voltage DCP enable
 * 0 = Disable HVDCP handshake (default)
 * 1 = Enable HVDCP handshake
 */
void
BQ25792::HVDCP_EN(byte data)
{
    FIELD_bit(Charger_Control + 2, HVDCP_EN_Mask, HVDCP_EN_Position, data);
}

/**
 * SFET control
 * The external ship FET control logic to force the device
 * enter different modes:
 * 00 = IDLE (default)
 * 01 = Shutdown Mode
 * 10 = Ship Mode
 * 11 = System Power Reset
 */
void
BQ25792::SDRV_CTRL(byte data)
{
    FIELD_bit(Charger_Control + 2, SDRV_CTRL_Mask, SDRV_CTRL_Position, data);
}

/**
 * Delay time added to the taking action of the
 * SFET control
 * 0 = Add 10s delay time (default)
 * 1 = Do NOT add 10s delay time
 */
void
BQ25792::SDRV_DLY(byte data)
{    
    FIELD_bit(Charger_Control + 2, SDRV_DLY_Mask, SDRV_DLY_Position, data);
}

/*=== [3] ===*/

/**
 * When wake up the device from ship mode, how much
 * time (tSM_EXIT) is required to pull low the QON pin.
 * 0 = 1s (default)
 * 1 = 15ms
 */
void
BQ25792::WKUP_DLY(byte data)
{
    FIELD_bit(Charger_Control + 3, WKUP_DLY_Mask, WKUP_DLY_Position, data);
}

/*=== [5] ===*/

void
BQ25792::SFET_PRESENT(byte data)
{    
    FIELD_bit(Charger_Control + 5, SFET_PRESENT_Mask, SFET_PRESENT_Position, data);
}

void
BQ25792::EN_IBAT(byte data)
{
    FIELD_bit(Charger_Control + 5, EN_IBAT_Mask, EN_IBAT_Position, data);
}

void
BQ25792::EN_EXTILIM(byte data)
{
    FIELD_bit(Charger_Control + 5, EN_EXTILIM_Mask, EN_EXTILIM_Position, data);
}

/* reg=0x2E name=ADC Control *************************************************/

/**
 * ADC Control
 * 0 = Disable
 * 1 = Enable
 */
void
BQ25792::ADC_EN(byte data)
{
    FIELD_bit(ADC_Control, ADC_EN_Mask, ADC_EN_Position, data);
}

/**
 * ADC conversion rate control
 * 0 = Continuous conversion
 * 1 = One shot conversion
 */
void
BQ25792::ADC_RATE(byte data)
{
    FIELD_bit(ADC_Control, ADC_RATE_Mask, ADC_RATE_Position, data);
}

/*****************************************************************************/

int BQ25792::getRegister(RegAddress addr, RegSize size, byte data[])
{
    byte len;
    byte k;

    len = Wire.requestFrom(I2C_ADDRESS, size, addr, 1, true);
    if (len > 0) {
        for (k = 0; Wire.available() && k < size; k++) {
            data[k] = Wire.read();
        }

        return 1;
    } else {
        return 0;
    }
}

int
BQ25792::get_Charger_Status(Reg_Charger_Status *charger_status)
{
    if (getRegister(Charger_Status, Charger_Status_Size, charger_status->raw)) {
        return 1;
    }
    return 0;
}

int
BQ25792::get_VBAT_ADC(uint16_t *vbat_adc)
{
    byte data[VBAT_ADC_Size];
    if (getRegister(VBAT_ADC, VBAT_ADC_Size, data)) {
        *vbat_adc = ((data[0] << 8) | data[1]);
        return 1;
    }
    return 0;
    /*
    byte len;
    byte k;
    byte data[VBAT_ADC_Size];

    len = Wire.requestFrom(I2C_ADDRESS, VBAT_ADC_Size, VBAT_ADC, 1, true);
    if (len > 0) {
        for (k = 0; Wire.available() && k < VBAT_ADC_Size; k++) {
            data[k] = Wire.read();
        }

        *vbat_adc = ((data[0] << 8) | data[1]);
        return 1;
    } else {
        return 0;
    }
    */
}


/*****************************************************************************/

void
BQ25792::printRegister()
{
    byte address = I2C_ADDRESS;
    byte len;

    byte i;
    byte k;

    byte data[I2C_DATA_SIZE_MAX];

    Serial.println("---------------------------------------------------------------------");
    Serial.print("Size=");
    Serial.println(REG_SIZE);

    for (i = 0; i < REG_SIZE; i++) {

        Serial.print("reg=");
        printHex(regDissector[i].address);
        Serial.print(" name=");
        Serial.print(regDissector[i].name);
        Serial.print(" size=");
        Serial.print(regDissector[i].size);

        Serial.print(" data=");
        /**
         * address: I2C direct address
         * size:    how many bytes are requested
         * address: register address
         */

        /* (uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop) */
        len = Wire.requestFrom(address, regDissector[i].size, regDissector[i].address, 1, true);
        if (len > 0) {
            for (k = 0; Wire.available() && k < regDissector[i].size; k++) {
                data[k] = Wire.read();
                printHex(data[k]);
                Serial.print(" ");
            }
            Serial.println("");
            if (regDissector[i].func != nullptr) {
                regDissector[i].func(data, regDissector[i].size);
            }
        } else {
            Serial.println("<null>");
        }

    }
}

/*****************************************************************************/

void
BQ25792::sendBytes(byte address, byte reg, byte *data, byte size)
{
    byte i = 0;
    Wire.beginTransmission(address);
    Wire.write(reg);
    while (i < size) {
        Wire.write(data[i]);
        i++;
    }
    Wire.endTransmission();
}


/**
 * I2C device found at address 0x50
 * I2C device found at address 0x6B
 */
void
BQ25792::scanDevices()
{
    byte error, address;
    int nDevices;

    Serial.println("I2C Scanner");
    Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++) {
        
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.

        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        switch (error) {
            case I2C_OK: {
                 Serial.print("I2C device found at address 0x");
                printAddress(address);
                nDevices++;
                break;
            }

            case I2C_DATA_TOO_LONG: {
                Serial.print("Data too long at address 0x");
                printAddress(address);
                break;
            }

            case I2C_NACK_ADDR: {
                // Not found, print nothing!
                break;
            }

            case I2C_NACK_DATA: {
                Serial.print("NACK at address 0x");
                printAddress(address);
                break;
            }

            case I2C_ERROR: {
                Serial.print("Error at address 0x");
                printAddress(address);
                break;
            }

            case I2C_TIMEOUT: {
                Serial.print("Timeout at address 0x");
                printAddress(address);
                break;
            }

            case I2C_BUSY: {
                Serial.print("Busy at address 0x");
                printAddress(address);
                break;
            }

            default: {
                Serial.print("Unknown error ");
                Serial.print(error);
                Serial.print(" at address 0x");
                printAddress(address);
            }
        }
    }
    if (nDevices == 0) {
        Serial.println("No I2C devices found");
    } else {
        Serial.println("done");
    }
}


/*****************************************************************************/

/* Main-loop */
void
BQ25792::loop()
{
    byte charger[Charger_Control_Size];
    bq25792.getRegister(BQ25792::Charger_Control, BQ25792::Charger_Control_Size, charger);
    if (m_timeout) {
        m_timeout = false;

        uint16_t vbat_adc;
        BQ25792::Reg_Charger_Status charger_status;
        int result;
        
        result = bq25792.get_VBAT_ADC(&vbat_adc);

        if (result) {
            if      (vbat_adc <= 13000)                         settings.batteryLevel = 1;
            else if (vbat_adc <= 14000 && vbat_adc > 13000)     settings.batteryLevel = 2;
            else if (vbat_adc <= 15000 && vbat_adc > 14000)     settings.batteryLevel = 3;
            else if (vbat_adc <= 16000 && vbat_adc > 15000)     settings.batteryLevel = 4;
            else if (                     vbat_adc > 16000)     settings.batteryLevel = 5;
        }

        result = bq25792.get_Charger_Status(&charger_status);
        if (result) {
            settings.inputSourceAttached = charger_status.VBUS_PRESENT_STAT;
        } else {
            Serial.println("bat error");
        }
    }
}

/* SoftwareTimerHandler in Interrupt mode */
void
BQ25792::handle()
{
    m_timeout = true;
}


/*****************************************************************************/

BQ25792 bq25792;

