
#include "rt/Pid_ctrl.h"

/*** put our own interface over the QuickPID module ***/
Pid_ctrl::Pid_ctrl(uint32_t sample_time_us, float kp, float ki, float kd)
  : qpi(&input, &output, &setpoint, kp, ki, kd, 1.0, 1.0, QuickPID::DIRECT)
{
  stop();
}

float Pid_ctrl::update(float sp, float pv)
{
  this->setpoint = sp;
  this->input = pv;
  qpi.Compute(); // reads from setpoint and writes to output
  return this->output;
  // ctrl.input = qpi.analogReadFast(PIN_INPUT);
  // analogWrite(PIN_OUTPUT, u_dac);
}

//TODO: fix QuickPID to work with floats!
void Pid_ctrl::set_output_limits(float out_min, float out_max)
{
  if (out_min >= out_max) return;
  qpi.SetOutputLimits(out_min, out_max);
}

void Pid_ctrl::set_tunings(float kp, float ki, float kd)
{
  qpi.SetTunings(kp, ki, kd);
}
