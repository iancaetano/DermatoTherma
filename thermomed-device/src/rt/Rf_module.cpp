
#include "rt/Rf_module.h"


static constexpr float limit(float x, float lol, float hil)
{
  return x<lol ? lol : (x>hil ? hil : x);
}

Rf_module::Rf_module()
{
  hw.begin();
  power_off();
}

// call periodically to update the state
void Rf_module::update()
{
  switch(state) {
    case State::off:
      break;

    case State::enabling:
      state_enabling();
      break;

    case State::running:
      state_running();
      break;

    case State::tripped:
      break;

    default:
      break;
  }
}

// moves from off or tripped to enabling
void Rf_module::enable()
{
  if(state == State::off || state == State::tripped) {
    trip_cause = Trip_cause::undef;
    state = State::enabling;
    // transition into first step of enabling sequence 
    rf_enabling_step = Rf_enabling_step::s1_stabilize_readings;
    pke_enable();
    

    seq_timer.restart_with_new_time(Delay::pke_start);
  }
}

// moves from any state to off
void Rf_module::power_off()
{
  pke_disable();
  
  module_reenable_lockout_timer.restart();
  state = State::off;
  trip_cause = Trip_cause::undef;
}

// no effect outside of running state
void Rf_module::set_DCDC_output(byte v)
{
  //if(state == State::running) {
    byte v_sat = limit(v, U_min, U_max);
    //DCDC_TPS.set_DCDC_output_hw(v_sat);
  //}
}

// only valid in running state
float Rf_module::get_primary_voltage_rms()
{
  float v_rms;
  if(state != State::off) {
    v_rms = G_dac_to_pri*get_DCDCOutput();
  } else {
    v_rms = (float)0;
  }
  return v_rms;
}

// only valid in running state
float Rf_module::get_IDC()
{
  if(state != State::off)
    return IDC;
  else
    return (float)0;
}

// only valid in running state
float Rf_module::get_load_resistance_estimate()
{
  if(state == State::running)
    return load_resistance_estimate;
  else
    return (float)Rl_max;
}

// only valid in running state
float Rf_module::get_power_estimate()
{
  if(state == State::running)
    return power_estimate;
  else
    return (float)0;
}

float Rf_module::get_max_primary_voltage_rms()
{
  return U_max;
}

void Rf_module::state_running() {
  update_readings();
  assure_safe_operating_point();
}

// this is a transitional state, implementing a timed sequence of steps
// to activate the RF module.
void Rf_module::state_enabling()
{
  // use safe default values
  uint32_t duration = 0;
  Rf_enabling_step next_step = Rf_enabling_step::s0_undef;

    // delay next transition until the current state's wait time is over
    if (seq_timer.is_expired()) {
      
      // switch cases define transition actions to next step & its duration
      switch (rf_enabling_step) {
        case Rf_enabling_step::s0_undef:
          break;
        case Rf_enabling_step::s1_stabilize_readings:
            next_step = Rf_enabling_step::s2_running;
            update_readings();
            state = State::running;
          break;
        case Rf_enabling_step::s2_running:
            // we should not arrive here
          break;
      }
      rf_enabling_step = next_step;
      seq_timer.restart_with_new_time(duration);
    }
}

// updates current measurement and estimates for other quantities
//TODO: add fiter for estimates
void Rf_module::update_readings()
{
  
  volatile float VDC = read_adc_VDC();
  IDC = read_adc_IDC();
  load_resistance_estimate = VDC/IDC;
  power_estimate = IDC * VDC;
  statRegDCDC = readStatReg();
}

void Rf_module::assure_safe_operating_point()
{
  Trip_cause cause = Trip_cause::undef;

  if (IDC < I_min_running)
    cause = Trip_cause::opamp_protection;
  else if (IDC > I_max)
    cause = Trip_cause::overcurrent_measured;
  else if (load_resistance_estimate < Rl_min)
    cause = Trip_cause::rl_estimate_too_low;
  else if (load_resistance_estimate > Rl_max)
    cause = Trip_cause::rl_estimate_too_low;
  else if (statRegDCDC & (1<<7))
    cause =Trip_cause::short_circuit;
  else if (statRegDCDC & (1<<6))
    cause =Trip_cause::overcurrent;
  else if (statRegDCDC & (1<<5))
    cause =Trip_cause::overvoltage;

  if(cause != Trip_cause::undef) {
    power_off();
    this->state = State::tripped;        
  }
  this->trip_cause = cause;

}

// ** hardware interface to RF module
//    (only set IO pin states with this functions)
// enables main supply for RF module




byte 
Rf_module::readStatReg()
{
  return 1;//DCDC_TPS.readStat();
}


void Rf_module::pke_enable()
{
  hw.pke_enable();
  dio.pke_enable = true;
}
// disables main supply for RF module
void Rf_module::pke_disable()
{
  hw.pke_disable();
  dio.pke_enable = false;
}
// Floats AD4870 ON_ pin -> if the AD4870 is already on, it activates
// the 1A current limit. Use this as default state at power up.

// Takes AD4870 ON_ to LOW -> if the its output was off it switches now on,
// but without current limit.





// returns the raw voltage at the ADC input for the primary side RMS current
float Rf_module::read_adc_VDC()
{
  dio.adc_VDC = hw.read_adc_VDC();
  return dio.adc_VDC;
}

float Rf_module::read_adc_IDC()
{
  dio.adc_IDC = hw.read_adc_IDC();
  return dio.adc_IDC;
}

float Rf_module::read_adc_phi()
{
  dio.adc_PHI = hw.read_adc_phi();
  return dio.adc_PHI;
}

// sets raw voltage DAC connected to the VGA -> controls RF amplitude


float Rf_module::get_DCDCOutput()
{
  return dio.DCDCOutput;
}
