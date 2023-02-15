
#include "rt/Safety_supervisor.h"

const uint32_t Safety_supervisor::temp_pv_update_timeout_ms = 100;
Rf_module::State test_state;

Safety_supervisor::Safety_supervisor(Rf_module& rfmod, Rt_system& rtsys)
  : rfmod{rfmod}, rtsys{rtsys}
{
  asses_and_act();
}

// checks patient safety and stops RF if safty conditions are violated
// check result with result()
// call periodically
Safety_supervisor::Assessment_result Safety_supervisor::asses_and_act()
{
  Violation vio = Violation::undef;
  last_assessment_result = Assessment_result::undef; 
  test_state = rfmod.get_state();
  switch (rfmod.get_state()) {
    case Rf_module::State::off:
    case Rf_module::State::tripped:
      vio = Violation::none;
      break;

    case Rf_module::State::enabling:
    case Rf_module::State::running:
      vio = Violation::none;
      break;

    case Rf_module::State::undef:
    default:      
      vio = Violation::firmware_bug;
      break;
  }

  if (vio != Violation::none) {
    last_assessment_result = Assessment_result::violated;
    rfmod.power_off();                              
  } else { 
    last_assessment_result = Assessment_result::safe;
  }

  last_violation = vio;
  return last_assessment_result;
}

Safety_supervisor::Violation Safety_supervisor::assess_on_states()
{
  Safety_supervisor::Violation vio = Safety_supervisor::Violation::none;
  
  if (rtsys.get_temp_pv() > sp.max_skin_temp_degc) {
    vio = Violation::max_skin_temp_exeeded;
  }
  
  uint32_t diff = millis() - rtsys.get_temp_pv_tstmp_ms();
  if (diff > sp.max_pv_update_period_ms) {
    vio = Violation::pv_temp_stalled;
  }

  if (rtsys.get_grid_connected()) {
    vio = Violation::grid_connected;
  }

  return vio;
}

void Safety_supervisor::get_result_of_last_assessment(
  Safety_supervisor::Assessment_result& res,
  Safety_supervisor::Violation& vio)
{
  res = this->last_assessment_result;
  vio = this->last_violation;
}
