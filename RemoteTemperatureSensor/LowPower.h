#include "hardware/pll.h"
#include "hardware/clocks.h"

void turnOffUsb() {
// CLK USB = 0MHz
    clock_stop(clk_usb);
    pll_deinit(pll_usb);
}

void turnOffADC() {
    // CLK ADC = 0MHz
    clock_stop(clk_adc);
}

void set_sys_clock_pll(uint32_t vco_freq, uint post_div1, uint post_div2) {
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                    48 * MHZ,
                    48 * MHZ);

    pll_init(pll_sys, 1, vco_freq, post_div1, post_div2);
    uint32_t freq = vco_freq / (post_div1 * post_div2);

    // Configure clocks
    // CLK_REF = XOSC (12MHz) / 1 = 12MHz
    clock_configure(clk_ref,
                    CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC,
                    0,  // No aux mux
                    12 * MHZ,
                    12 * MHZ);

    // CLK SYS = PLL SYS (125MHz) / 1 = 125MHz
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                    freq, freq);

    clock_configure(clk_peri,
                    0,  // Only AUX mux on ADC
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                    48 * MHZ,
                    48 * MHZ);

  //  CLK RTC = ideally XOSC (12MHz) / 256 = 46875Hz but could be rosc
      uint clk_rtc_src = CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_XOSC_CLKSRC;
      uint src_hz = XOSC_MHZ * MHZ;

    clock_configure(clk_rtc,
                    0, // No GLMUX
                    clk_rtc_src,
                    src_hz,
                    46875);  
}

bool check_sys_clock_khz(uint32_t freq_khz, uint *vco_out, uint *postdiv1_out, uint *postdiv_out) {
  uint crystal_freq_khz = clock_get_hz(clk_ref) / 1000;
  for (uint fbdiv = 320; fbdiv >= 16; fbdiv--) {
    uint vco = fbdiv * crystal_freq_khz;
    if (vco < 400000 || vco > 1600000) continue;
    for (uint postdiv1 = 7; postdiv1 >= 1; postdiv1--) {
      for (uint postdiv2 = postdiv1; postdiv2 >= 1; postdiv2--) {
        uint out = vco / (postdiv1 * postdiv2);
        if (out == freq_khz && !(vco % (postdiv1 * postdiv2))) {
          *vco_out = vco * 1000;
          *postdiv1_out = postdiv1;
          *postdiv_out = postdiv2;
          return true;
        }
      }
    }
  }
  return false;
}

/*! \brief Attempt to set a system clock frequency in khz
 *  \ingroup pico_stdlib
 *
 * Note that not all clock frequencies are possible; it is preferred that you
 * use src/rp2_common/hardware_clocks/scripts/vcocalc.py to calculate the parameters
 * for use with set_sys_clock_pll
 *
 * \param freq_khz Requested frequency
 * \param required if true then this function will assert if the frequency is not attainable.
 * \return true if the clock was configured
 */
 bool set_sys_clock_khz(uint32_t freq_khz, bool required)
 {
  uint vco, postdiv1, postdiv2;
  if (check_sys_clock_khz(freq_khz, &vco, &postdiv1, &postdiv2)) {
    set_sys_clock_pll(vco, postdiv1, postdiv2);
    //Recalibrate Delay()
    SysTick->LOAD = freq_khz - 1;    
    return true;
  } else if (required) {
    panic("System clock of %u kHz cannot be exactly achieved", freq_khz);
  }
  return false;
}

void set_lowPower() {
  set_sys_clock_khz(10000,true);  //10MHz
  turnOffUsb();
  turnOffADC();
}
