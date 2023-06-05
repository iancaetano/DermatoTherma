
#include "rt/Rf_module.h"
#include  "main.h"
#include "Settings.h"
#include "sound.h"

static constexpr float limit(float x, float lol, float hil)
{
  return x<lol ? lol : (x>hil ? hil : x);
}

Rf_module::Rf_module()
{
  hw.begin();
  set_dac_voltage(0.1);
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
    set_primary_voltage_rms(0.1);
    pke_enable();
    seq_timer.restart_with_new_time(Delay::pke_start);
    state = State::running;
    Sound.TreatmentTone(settings.temperatureActual);
    Sound.sound_enable();
    
  }
}

// moves from any state to off
void Rf_module::power_off()
{
  rtsys.stop_treatment();
  settings.rfPowerOn = 0;
  set_dac_voltage(0.1);
  pke_disable();
  module_reenable_lockout_timer.restart();
  state = State::off;
  trip_cause = Trip_cause::undef;
  resetValues();
  Sound.sound_disable();

}

// no effect outside of running state
void Rf_module::set_primary_voltage_rms(float v)
{
  //if(state == State::running) {
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
float Rf_module::get_primary_current()
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
  Sound.TreatmentTone(settings.temperatureActual);
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
  vdc = read_RFVDC();
  idc = read_RFI();
  volatile float phi = read_RFPHI();
  load_resistance_estimate = vdc/idc;
  power_estimate = idc * vdc;
}

void Rf_module::assure_safe_operating_point()
{
  Trip_cause cause = Trip_cause::undef;


  if (idc > I_max)
    cause = Trip_cause::overcurrent_measured;
  else if(settings.temperatureActual<30)
    cause = Trip_cause::temp_low;
  else if(settings.temperatureActual>55)
    cause = Trip_cause::temp_high;
  /*
  else if (load_resistance_estimate < Rl_min)
    cause = Trip_cause::rl_estimate_too_low;

  else if (load_resistance_estimate > Rl_max)
    cause = Trip_cause::rl_estimate_too_low;
*/

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
float Rf_module::read_RFVDC()
{
  dio.RF_VDC = hw.read_adc_VDC();
  return dio.RF_VDC;
}

float Rf_module::read_RFI()
{
  dio.RF_IDC = hw.read_adc_IDC();
  return dio.RF_IDC;
}

float Rf_module::read_RFPHI()
{
  dio.RF_PHI = hw.read_adc_phi();
  return dio.RF_PHI;
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


void 
Rf_module::resetValues()
{
  idc = 0;
  vdc = 0;
}