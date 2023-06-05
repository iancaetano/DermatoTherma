
#pragma once

#include <Arduino.h>

#include "rt/Rf_hardware.h"
#include "rt/Countdown_timer.h"
#include "sound.h"

class Rf_module {
  public:

    float read_RFVDC();
    float read_RFI();
    float read_RFPHI();

    volatile float idc;
    volatile float vdc;

    void set_dac_voltage(float v);
    enum class State {undef, off, enabling, running, tripped};
    enum class Trip_cause {undef, opamp_protection, overcurrent_measured,
                           rl_estimate_too_low, rl_estimate_too_high, temp_low, temp_high};

    Rf_module();

    void update(); // call periodically to update the state

    inline State get_state() { return state; }
    inline Trip_cause get_trip_cause() { return trip_cause; }

    void enable(); // moves from off or tripped to enabling
    void power_off(); // moves from any state to off

    // no effect outside of running state
    void set_primary_voltage_rms(float v);

    // only valid in running state
    float get_primary_voltage_rms();
    float get_primary_current();
    float get_load_resistance_estimate();
    float get_power_estimate();

    float get_max_primary_voltage_rms();

    
  private:
    enum class Rf_enabling_step {
      s0_undef,
      s1_power_up,
      s2_opamp_enable,
      s3_enable_current_limit,
      s4_stabilize_readings,
      s5_running,
    };

    // *** time constants in us
    // time from disabling PKE until module is considered powerless
    class Delay {
      public:
        static const uint32_t pke_shutdown = 10;
        static const uint32_t pke_start = 10;
        static const uint32_t opamp_enable = 10;
        static const uint32_t current_limit_enable = 10;
        static const uint32_t stabilize_readings = 50;
    };

    // RF module parameters
    //TODO: convert to const?
    float T_ratio = 9.0; // impedance transformation ratio
    float Rl_min = 100.0; // minimal supported load resistance
    float Rl_max = 250.0; // maximal supported load resistance
    float V_vga = 0.041043; //
    float U_min = 3;   // minimum voltage at amplifier output (RMS) (make > 0V)
    float U_max = 18; // maximum possible voltage at amplifier output (RMS) (note: could be dominated by DAC limit)
    float I_min_running = 0.020; // minimum measured current when running [A_RMS]
    float I_max = 1.5;
    float G_tr_pri_to_sec = 3; // gain: primary voltage -> secundary voltage  [V/V]
    float G_i_pri_to_adc = 0.2614; // gain: primary RMS current -> ADC voltage [V/A_RMS]
    float G_dac_to_pri = 102.81; // gain:  DAC voltage -> primary voltage [V_RMS/V]

    Rf_hardware hw;
    State state;
    Trip_cause trip_cause;
    Rf_enabling_step rf_enabling_step;

    Countdown_timer module_reenable_lockout_timer{Delay::pke_start};
    Countdown_timer seq_timer{0};

    float primary_current_rms; // A
    float load_resistance_estimate; // Ohm
    float power_estimate; // W
  
    #define nArrayElements  10
    int filterCount_VDC;
    int filterCount_IDC;
    int filterCount_PHI;

    float VDC_Array[nArrayElements];
    float IDC_Array[nArrayElements];
    float PHI_Array[nArrayElements];

    void resetValues();

    inline void state_running();

    // this is a transitional state, implementing a timed sequence of steps
    // to activate the RF module.
    inline void state_enabling();

    // updates current measurement and estimates for other quantities
    //TODO: add fiter for estimates
    inline void update_readings();

    inline void assure_safe_operating_point();

    // ** hardware interface to RF module
    //    (only set IO pin states with this functions)
    // enables main supply for RF module
    inline void pke_enable();
    // disables main supply for RF module
    inline void pke_disable();



    // sets raw voltage DAC connected to the VGA -> controls RF amplitude
    
    inline float get_dac_voltage();

    // control via debugger
    volatile struct Dummy_ios {
      int pke_enable;
      int opamp_on_input_float; // low -> pull low
      float dac_voltage;
      float RF_VDC; // input
      float RF_IDC;
      float RF_PHI;
    } dio;
};
