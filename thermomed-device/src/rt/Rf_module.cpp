
#include "rt/Rf_module.h"

static constexpr float limit(float x, float lol, float hil)
{
  return x<lol ? lol : (x>hil ? hil : x);
}

Rf_module::Rf_module()
{
  hw.begin();
  power_off();

  filterCount_VDC = 0;
  filterCount_IDC = 0;
  filterCount_PHI = 0;

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
    // transition into first first step of enabling sequence 
    set_primary_voltage_rms(0);
    pke_enable();
    seq_timer.restart_with_new_time(Delay::pke_start);
  }
}

// moves from any state to off
void Rf_module::power_off()
{
  set_dac_voltage(0);
  pke_disable();
  module_reenable_lockout_timer.restart();
  state = State::off;
  trip_cause = Trip_cause::undef;
}

// no effect outside of running state
void Rf_module::set_primary_voltage_rms(float v)
{
  //if(state == State::running) {
    float v_sat = limit(v, U_min, U_max);
    float RF_VDC = v_sat/(G_dac_to_pri*V_vga);
    set_dac_voltage(v);
  //}
}

// only valid in running state
float Rf_module::get_primary_voltage_rms()
{
  float v_rms;
  if(state != State::off) {
    v_rms = G_dac_to_pri*get_dac_voltage();
  } else {
    v_rms = (float)0;
  }
  return v_rms;
}

// only valid in running state
float Rf_module::get_primary_current_rms()
{
  if(state != State::off)
    return primary_current_rms;
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
  seq_timer.restart_with_new_time(duration);
}


// updates current measurement and estimates for other quantities
//TODO: add fiter for estimates
void Rf_module::update_readings()
{
  volatile float dac = get_dac_voltage();
  volatile float vprim_rms = V_vga*dac*G_dac_to_pri;
  volatile float vadc = read_RFVDC_voltage();
  primary_current_rms = vadc*G_i_pri_to_adc;
  load_resistance_estimate = vprim_rms / primary_current_rms * T_ratio;
  power_estimate = primary_current_rms * vprim_rms;
}

void Rf_module::assure_safe_operating_point()
{
  Trip_cause cause = Trip_cause::undef;

  if (primary_current_rms < I_min_running)
    cause = Trip_cause::opamp_protection;
  else if (primary_current_rms > I_max)
    cause = Trip_cause::overcurrent_measured;
  else if (load_resistance_estimate < Rl_min)
    cause = Trip_cause::rl_estimate_too_low;
  else if (load_resistance_estimate > Rl_max)
    cause = Trip_cause::rl_estimate_too_low;

  if(cause != Trip_cause::undef) {
    power_off();
    this->state = State::tripped;        
  }
  this->trip_cause = cause;

}

// ** hardware interface to RF module
//    (only set IO pin states with this functions)
// enables main supply for RF module
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





// returns the raw voltage at the ADC input for the primary side RMS current
float Rf_module::read_RFVDC_voltage()
{
  dio.RF_VDC = hw.read_adc_VDC();
  return dio.RF_VDC;
}

// sets raw voltage DAC connected to the VGA -> controls RF amplitude
void Rf_module::set_dac_voltage(float v)
{
  hw.set_dac_voltage(v);
  dio.dac_voltage = v;
}

float Rf_module::get_dac_voltage()
{
  return dio.dac_voltage;
}
