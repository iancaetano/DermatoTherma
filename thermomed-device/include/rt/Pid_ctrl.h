
#pragma once

#include <Arduino.h>
#include "QuickPID/QuickPID.h"

// put our own interface over the QuickPID module
class Pid_ctrl {
  public:
    Pid_ctrl(uint32_t sample_time_us, float kp, float ki, float kd);

    inline void restart()
    {
      this->output = 0;
      qpi.SetMode(QuickPID::AUTOMATIC); // turn the PID on
    }
    inline void stop() { qpi.SetMode(QuickPID::MANUAL); }

    float update(float sp, float pv);

    //TODO: fix QuickPID to work with floats!
    void set_output_limits(float out_min, float out_max);
    void set_tunings(float kp, float ki, float kd);

  private:
    QuickPID qpi;
    float setpoint, input, output;//Define Variables we'll be connecting to
};
