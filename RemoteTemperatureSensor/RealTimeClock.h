#include "rtc_api.h"

/* these are not currently covered by the API */
extern "C" 
{
  typedef void(* rtc_callback_t )(void);
  void rtc_set_alarm      (datetime_t *t, rtc_callback_t user_callback);
  void rtc_disable_alarm (void);
  void rtc_enable_alarm (void);
  bool rtc_set_datetime(datetime_t *t);
}

static volatile bool awake;

static void rtc_callback(void) {
    awake = true;
    return;
}

void setup_rtc() {
  // Start the RTC
    rtc_init();

    datetime_t t_start = {
              .year  = 2022,
              .month = 06,
              .day   = 25,
              .dotw  = 6, // 0 is Sunday, so 5 is Friday
              .hour  = 8,
              .min   = 0,
              .sec   = 0  
      };

    //Write initial date
    rtc_set_datetime(&t_start);

    // Wait for it to start
    while(!rtc_isenabled()) 
    { }
}

void waitfor_rtc(int seconds) {
  int min = seconds / 60;
  int sec = seconds % 60;

  datetime_t t_alarm = {
            .year  = 2022,
            .month = 06,
            .day   = 25,
            .dotw  = 6, // 0 is Sunday, so 5 is Friday
            .hour  = 8,
            .min   = min,
            .sec   = sec   //5s later than the initial time
    };

  rtc_set_alarm(&t_alarm,&rtc_callback);

  while(!awake) {}

  rtc_disable_alarm ();
}