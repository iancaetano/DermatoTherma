
#include "rt/Countdown_timer.h"


// When using Arduino milis() as time basis, consider:
//  - Delay times can not be larger than 49 days.
//  - A member funtion should be called at least once latest after 49 days
//    after a timer expired. Otherwise the timer may miss it's expiration. 
Countdown_timer::Countdown_timer()
  : Countdown_timer::Countdown_timer{0} {}
Countdown_timer::Countdown_timer(uint32_t countdown_lenght_ms)
{
  this->countdown_lenght_ms = countdown_lenght_ms;
  force_expiration();
}

// starts or restarts the timer with last countdown time
void Countdown_timer::restart()
{
  time_left_ms = countdown_lenght_ms;
  sys_time_last_call_ms = get_system_time_ms();
}

// starts or restarts the timer with a new countown time
void Countdown_timer::restart_with_new_time(uint32_t time)
{
  countdown_lenght_ms = time;
  restart();
}

void Countdown_timer::force_expiration()
{
  update_times(); // update last call
  time_left_ms = 0;
}

bool Countdown_timer::is_expired()
{
  update_times();
  return time_left_ms == 0;
}

uint32_t Countdown_timer::get_time_left_ms()
{
  update_times();
  return time_left_ms;
}

uint32_t Countdown_timer::get_system_time_ms()
{
  return millis();
}

void Countdown_timer::update_times()
{
  uint32_t delta;

  uint32_t curr_sys = get_system_time_ms();
  uint32_t last_sys = sys_time_last_call_ms;

  // compute time difference since last call
  delta = curr_sys - last_sys; // overflow handled by unsigned artihmetic

  if (delta >= time_left_ms) {
    time_left_ms = 0; // countdown reached zero
  } else {
    time_left_ms = time_left_ms - delta;  // update remaining time
  }

  this->sys_time_last_call_ms = curr_sys;
}