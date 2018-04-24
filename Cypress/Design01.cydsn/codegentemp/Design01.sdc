# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Users\Elias g\FDRM-kl25z\Cypress\Design01.cydsn\Design01.cyprj
# Date: Sat, 21 Apr 2018 20:17:38 GMT
#set_units -time ns
create_clock -name {ADC_intClock(FFB)} -period 500 -waveform {0 250} [list [get_pins {ClockBlock/ff_div_6}]]
create_clock -name {I2CP_SCBCLK(FFB)} -period 625 -waveform {0 312.5} [list [get_pins {ClockBlock/ff_div_2}]]
create_clock -name {CyRouted1} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/dsi_in_0}]]
create_clock -name {CyWCO} -period 30517.578125 -waveform {0 15258.7890625} [list [get_pins {ClockBlock/wco}]]
create_clock -name {CyLFClk} -period 30517.578125 -waveform {0 15258.7890625} [list [get_pins {ClockBlock/lfclk}]]
create_clock -name {CyILO} -period 31250 -waveform {0 15625} [list [get_pins {ClockBlock/ilo}]]
create_clock -name {CyECO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/eco}]]
create_clock -name {CyIMO} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyHFClk} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/hfclk}]]
create_clock -name {CySysClk} -period 20.833333333333332 -waveform {0 10.4166666666667} [list [get_pins {ClockBlock/sysclk}]]
create_generated_clock -name {ADC_intClock} -source [get_pins {ClockBlock/hfclk}] -edges {1 25 49} [list]
create_generated_clock -name {timer_clock} -source [get_pins {ClockBlock/hfclk}] -edges {1 49 97} [list [get_pins {ClockBlock/udb_div_1}]]
create_generated_clock -name {I2CP_SCBCLK} -source [get_pins {ClockBlock/hfclk}] -edges {1 31 61} [list]
create_generated_clock -name {Clock_1} -source [get_pins {ClockBlock/hfclk}] -edges {1 49 97} [list [get_pins {ClockBlock/udb_div_3}]]


# Component constraints for C:\Users\Elias g\FDRM-kl25z\Cypress\Design01.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Users\Elias g\FDRM-kl25z\Cypress\Design01.cydsn\Design01.cyprj
# Date: Sat, 21 Apr 2018 20:17:28 GMT
