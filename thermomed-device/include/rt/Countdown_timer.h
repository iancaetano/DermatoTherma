#pragma once

#include <Arduino.h>

// When using Arduino milis() as time basis, consider:
//  - Delay times can not be larger than 49 days.
//  - A member funtion should be called at least once latest after 49 days
//    after a timer expired. Otherwise the timer may miss it's expiration. 
class Countdown_timer {
  public:
    Countdown_timer();
    Countdown_timer(uint32_t countdown_lenght_ms);
    
    void restart(); // starts or restarts the timer with last countdown time
    void restart_with_new_time(uint32_t time);
    void force_expiration();
    bool is_expired();
    uint32_t get_time_left_ms();

  private:
    uint32_t get_system_time_ms();
    void update_times();

    uint32_t sys_time_last_call_ms;
    uint32_t countdown_lenght_ms;
    uint32_t time_left_ms;
};