#include "rt/Rt_system.h"

uint32_t start_delay_ms = 3000;
static float delta_temp_min = 1;   //VERAENDERT WAR AUF 0.1
static float sp_offset = 2.;

/***
  hosts all realtime code that is execute in the realtime context
  offers a safe interface for the main loop

  external inputs:
    skin temperature -> for controlel rand patient safety
    sp temperature -> used as controller setpoint
    start/stop treatment -> requests and stops treatment
    grid status -> used for patient safety
***/
Rt_system::Rt_system()
  : rfmod{},
    sup{rfmod, *this},
    ctrl{&rfmod}
  {
    init();
  }

void Rt_system::init(void)
{
  rfmod.power_off();
  ctrl.stop();
  rt_status = Rt_status::off;

  /*** initalize main loop IOs ***/
  ivs_in_use = false;
  ivs.grid_connected = true;
  ivs.temp_pv = true;
  ivs.temp_pv_tstmp_ms = 0;
  ivs.temp_sp = 0;
  ivs.start_treatment = false;
  ivs.stop_treatment = false;
  read_inputs(); // apply also to internal copy
  /******/
}

/*** inputs/outputs for main loop (use only from main loop context) ***/
void Rt_system::set_grid_connected(bool b)
{
  ivs_in_use = true;
  ivs.grid_connected = b;
  ivs_in_use = false;
}

void Rt_system::set_temp_pv(float t)
{
  ivs_in_use = true;
  ivs.temp_pv = t;
  ivs.temp_pv_tstmp_ms = millis();
  ivs_in_use = false;
}

void Rt_system::set_temp_sp(float t)
{
  ivs_in_use = true;
  ivs.temp_sp = t;
  ivs_in_use = false;
}

void Rt_system::start_treatment()
{
  ivs_in_use = true;
  ivs.start_treatment = true;
  ivs_in_use = false;
}

void Rt_system::stop_treatment()
{
  ivs_in_use = true;
  ivs.stop_treatment = true;
  ivs_in_use = false;
} 

void Rt_system::get_status(struct Rt_system::Rt_out &s) // output
{
  ovs_in_use = true;
  s = ovs;
  ovs_in_use = false;
}


// main RT loop, called by timer at 500Hz
void Rt_system::rt_callback()
{
  bool new_ml_vals;
  float u_sat_cv;
  Safety_supervisor::Assessment_result res;
  bool hardware_err;
  bool safety_err;



  new_ml_vals = read_inputs(); // receive inputs from main loop

  rfmod.update(); // update RF module state (read inputs/make sure it runs safe)
  hardware_err = (rfmod.get_state() == Rf_module::State::tripped);

  res = sup.asses_and_act();
  safety_err = (res != Safety_supervisor::Assessment_result::safe);

  // execute controller part slightly later if main loop is updating inputs
  if(new_ml_vals) {
    bool abort = false;
    bool check_for_change;
    float dtemp;

    switch(rt_status) {
      case Rt_status::off:
        if(iv.start_treatment) {
          rfmod.enable();
          ctrl.restart();
          rt_status = Rt_status::treating;

          this->temp_last = iv.temp_pv;
          this->start_time.restart_with_new_time(start_delay_ms);
        }
        break;

      case Rt_status::treating:
        check_for_change = iv.temp_sp - iv.temp_pv > sp_offset;
        if(start_time.is_expired() && check_for_change) {
          dtemp = iv.temp_pv-temp_last;
          if (dtemp < delta_temp_min) {
            abort = false;//true;
          } else {
            this->temp_last = iv.temp_pv;
            start_time.restart();
          }
        }
        
        /*
        if(hardware_err || safety_err || abort) {
          ctrl.stop();
          rfmod.power_off();
          rt_status = Rt_status::error;
        
        } */else if (iv.stop_treatment) {
          ctrl.stop();
          rfmod.power_off();
          rt_status = Rt_status::off;
        } else {
          Rf_module::State rfmodstate = rfmod.get_state();
          if(rfmodstate == Rf_module::State::running) {
            u_sat_cv = ctrl.update(iv.temp_sp, iv.temp_pv);
            dummy_Y = u_sat_cv;
            rfmod.set_primary_voltage_rms(u_sat_cv);
          }
        }
        break;

      case Rt_status::error:
        if(iv.start_treatment) {
          ctrl.restart();
          rt_status = Rt_status::treating;
        }
        if(iv.stop_treatment) {
          //TODO: stops
          rt_status = Rt_status::off;
        }
        break;

      case Rt_status::undef:
      default:
        break;
    }
    write_outputs(); // prepare outputs for main loop
  }

}

/*** savely access main loop IOs ***/
// executes in RT context
bool Rt_system::read_inputs()
{
  bool updated = false;

  // update input vars only if main loop is not in a critical section
  if(!ivs_in_use) {
    iv = ivs;

    // reset single-event inputs
    ivs.start_treatment = false;
    ivs.stop_treatment = false;

    updated = true;
  }

  return updated;
}

// executes in RT context
bool Rt_system::write_outputs()
{
  bool updated = false;

  // update output vars only if main loop is not in the critical section
  if(!ovs_in_use) {
    ovs.rt_status = rt_status;
    ovs.rfmod_state = rfmod.get_state();
    ovs.rfmod_trip_cause = rfmod.get_trip_cause();
    ovs.power_estimate = rfmod.get_power_estimate();
    ovs.load_resistance_estimate = rfmod.get_load_resistance_estimate();
    sup.get_result_of_last_assessment(ovs.safety_state, ovs.violation_cause);
    updated = true;
  }

  return updated;
}
/******/
