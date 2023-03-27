
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
    // transition into first first step of enabling sequence 
    rf_enabling_step = Rf_enabling_step::s1_power_up;
    set_primary_voltage_rms(0);
    pke_enable();
    seq_timer.restart_with_new_time(Delay::pke_start);
  }
}

// moves from any state to off
void Rf_module::power_off()
{
  set_DCDC_output(0);
  set_opamp_on_input_float();
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
    float adc_voltage = v_sat/(G_dac_to_pri*V_vga);
    set_DCDC_output(adc_voltage);
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
  Rf_enabling_step next_step = Rf_enabling_step::s0_undef;

    // delay next transition until the current state's wait time is over
    if (seq_timer.is_expired()) {
      
      // switch cases define transition actions to next step & its duration
      switch (rf_enabling_step) {
        case Rf_enabling_step::s0_undef:
          break;
        case Rf_enabling_step::s1_power_up:
            next_step = Rf_enabling_step::s2_opamp_enable;
            set_opamp_on_input_pull_low();// activate opamp
            duration = Delay::opamp_enable;
          break;
        case Rf_enabling_step::s2_opamp_enable:
            next_step = Rf_enabling_step::s3_enable_current_limit;
            set_opamp_on_input_float();
            duration = Delay::current_limit_enable;
          break;
        case Rf_enabling_step::s3_enable_current_limit:
            next_step = Rf_enabling_step::s4_stabilize_readings;
            set_primary_voltage_rms(U_min);
            duration = Delay::stabilize_readings;
          break;
        case Rf_enabling_step::s4_stabilize_readings:
            next_step = Rf_enabling_step::s5_running;
            update_readings();
            state = State::running;
          break;
        case Rf_enabling_step::s5_running:
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
  volatile float dac = get_dac_voltage();
  volatile float vprim_rms = V_vga*dac*G_dac_to_pri;
  volatile float vadc = read_adc_VDC();
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
// Floats AD4870 ON_ pin -> if the AD4870 is already on, it activates
// the 1A current limit. Use this as default state at power up.
void Rf_module::set_opamp_on_input_float()
{
  hw.set_opamp_on_input_float();
  dio.opamp_on_input_float = true;
}
// Takes AD4870 ON_ to LOW -> if the its output was off it switches now on,
// but without current limit.
void Rf_module::set_opamp_on_input_pull_low()
{
  hw.set_opamp_on_input_pull_low();
  dio.opamp_on_input_float = false;
}


void Rf_module::set_debug_pin_state(bool state)
{
  hw.set_debug_pin_state(state);
}

// returns the raw voltage at the ADC input for the primary side RMS current
float Rf_module::read_adc_VDC()
{
  dio.adc_voltage = hw.read_adc_VDC();
  return dio.adc_voltage;
}

// sets raw voltage DAC connected to the VGA -> controls RF amplitude
void Rf_module::set_DCDC_output(float v)
{
  hw.set_DCDC_output(v);
  dio.dac_voltage = v;
}

float Rf_module::get_dac_voltage()
{
  return dio.dac_voltage;
}
