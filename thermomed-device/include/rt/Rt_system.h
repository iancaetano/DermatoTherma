
#pragma once

#include "rt/Rf_module.h"
#include "rt/Safety_supervisor.h"
#include "rt/Temp_ctrl.h"

extern uint32_t start_delay_ms;

// Runs in its own context by calling rt_callback().
class Rt_system {
  public:
    bool dcdcStartFlag;
    Rt_system();
    // state indicator for main loop
    enum class Rt_status {undef, off, treating, error}; 
    // use this struct only for communication with main loop
    struct Rt_out {
        Rt_status rt_status;
        Rf_module::State rfmod_state; // valid if rt_status is error
        Rf_module::Trip_cause rfmod_trip_cause;
        float power_estimate;
        float load_resistance_estimate;
        Safety_supervisor::Assessment_result safety_state; // valid if rt_status is error
        Safety_supervisor::Violation violation_cause;
    };

    // Called at instantiation.
    // Can be called again later the reset the RT subsystem.
    void init();

    void rt_callback(); // called by timer at 500Hz

    /*** interface to main loop ***/
    // inputs
    void set_grid_connected(bool b);
    void set_temp_pv(float t); // measured skin temperature
    void set_temp_sp(float t); // target skin temperature (setpoint)
    void start_treatment();
    void stop_treatment();
    
    // outputs (status variables)
    void get_status(struct Rt_out &o);
    /******/
    
    /*** give other RT objects access to inputs (dont call from main loop) ***/
    inline bool get_grid_connected() { return iv.grid_connected; }
    inline float get_temp_pv() { return iv.temp_pv; }
    inline uint32_t get_temp_pv_tstmp_ms() { return iv.temp_pv_tstmp_ms; }
    inline float get_temp_sp() { return iv.temp_sp; }
    /******/

  private:
    /*** components of RT subsystem ***/
    // HAL for RF module
    Rf_module rfmod; 

    // checks vairous parameters and stops
    // RF module (treatment?) if not safe for patient
    Safety_supervisor sup;

    // controls skin temperature during treatment
    Temp_ctrl ctrl;
    /******/

    // state of RT subsystem, internally relavant,
    // but also indicator for main loop
    Rt_status rt_status;

    /*** input from main loop ***/
    // use this struct only for communication with main loop
    // fields are updated via RT_system's input methods
    struct Input_vars {
      bool grid_connected;
      float temp_pv;
      uint32_t temp_pv_tstmp_ms;
      float temp_sp;
      bool start_treatment;
      bool stop_treatment;
      
    };
    volatile bool ivs_in_use;// mutex var, written by main loop, read by rt loop
    struct Input_vars ivs; // shared input variables, written by main loop
    struct Input_vars iv; // internal copy
    /******/

    /*** output for main loop ***/
    volatile bool ovs_in_use;// mutex var, written by main loop, read by rt loop
    struct Rt_out ovs; // shared copy of output variables, read by main loop
    /******/

    Countdown_timer start_time{start_delay_ms};
    float temp_last;
    bool checked;

    byte under_boundary;
    byte upper_boundary;
   

    inline bool read_inputs();
    inline bool write_outputs(); // prepare outputs for main loop
    byte convertToHex(float v);
};
