
#pragma once

#include "rt/Rf_module.h"
#include "rt/Pid_ctrl.h"

// skin temperature controller
class Temp_ctrl {
  public:
    Temp_ctrl(Rf_module *rfmod);
    inline void restart() { pi.restart(); }
    inline void stop() { pi.stop(); }
    float update(float t_sp, float t_pv); // returns new DCDC_out

  private:
    Rf_module *rfmod;

    Pid_ctrl pi;
    uint16_t pi_sample_period_ms = 20;  //orig 50
    
    float rl_dummy_prim = 100./9.; // serves as an estimate for the true load
};
