
#pragma once

#include "rt/Rf_module.h"

class Rt_system; // declare for Safety_supervisor

// checks various conditions to guarantee the patients safety during treatment
class Safety_supervisor {
  public:
    enum class Assessment_result {undef, safe, violated};
    enum class Violation {
      undef,
      none,
      firmware_bug,
      //initial_temp_too_low,
      max_skin_temp_exeeded,
      //temp_change_too_slow,
      //temp_change_too_fast,
      grid_connected,
      pv_temp_stalled, // -> handset disconnected
      //main_loop_stalled
    };
    Safety_supervisor(Rf_module& rfmod, Rt_system& rtsys);

    // checks patient safety and stops RF if safty conditions are violated
    // check result with result()
    // call periodically
    Assessment_result asses_and_act();
    void get_result_of_last_assessment(Assessment_result& res, Violation& vio);

  private:
    inline Violation assess_on_states();

    Rf_module& rfmod;
    Rt_system& rtsys;
    Assessment_result last_assessment_result = Assessment_result::undef;
    Violation last_violation = Violation::undef;
    
    static const uint32_t temp_pv_update_timeout_ms;
    Countdown_timer temp_pv_update_timer;

    struct Safety_parameter {
      float max_skin_temp_degc = 65;
      uint32_t max_pv_update_period_ms = 500;
    } sp;
};

#include "rt/Rt_system.h"
