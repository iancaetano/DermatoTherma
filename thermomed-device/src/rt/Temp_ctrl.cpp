
#include "rt/Temp_ctrl.h"

// static float kp = 1.25;
// static float ki = 0.0015;

//static float kp = 0.7;    
//static float ki = 0.0175;   
//static float kd = 0.25;     

static float kp = 0.4;
static float ki = 0.035;
static float kd = 0.25;

//TODO probably better no var for limits here. use rf module!
static float power_cv_min = 0.1; 
static float power_cv_max = 6;    //TODO use limit from rfmod

/*** Supervise treatment using, RF module and RT system ***/
Temp_ctrl::Temp_ctrl(Rf_module *rfmod)
  : pi(1000*(uint32_t)pi_sample_period_ms, kp, ki, kd)
{
  this->stop();
  this->rfmod = rfmod;
  //power_cv_max = pow(rfmod->get_max_primary_voltage_rms(), (float)2)/rl_dummy_prim;
  pi.set_output_limits(power_cv_min, power_cv_max); // in terms of power //TODO use limit from rfmod
}

float Temp_ctrl::update(float t_sp, float t_pv)
{ 
  volatile float p_sat;
  volatile float u_sat; // voltage on primary side


  p_sat =  pi.update(t_sp, t_pv);
  // convert from power cv to cv output to present a linear
  // behaving plant to the PI      
  u_sat = sqrt(p_sat * rl_dummy_prim);
  return u_sat;
}
/****/
