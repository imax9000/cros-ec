/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
/* EC for Falco board configuration */

#include "adc.h"
#include "board.h"
#include "chip_temp_sensor.h"
#include "common.h"
#include "ec_commands.h"
#include "extpower.h"
#include "gpio.h"
#include "i2c.h"
#include "keyboard_scan.h"
#include "lid_switch.h"
#include "lm4_adc.h"
#include "peci.h"
#include "power_button.h"
#include "registers.h"
#include "switch.h"
#include "temp_sensor.h"
#include "timer.h"
#include "tmp006.h"
#include "util.h"
#include "x86_power.h"

/* GPIO signal list.  Must match order from enum gpio_signal. */
const struct gpio_info gpio_list[GPIO_COUNT] = {
	/* Inputs with interrupt handlers are first for efficiency */
	{"POWER_BUTTON_L",       LM4_GPIO_A, (1<<2), GPIO_INT_BOTH,
	 power_button_interrupt},
	{"LID_OPEN",             LM4_GPIO_A, (1<<3), GPIO_INT_BOTH,
	 lid_interrupt},
	{"AC_PRESENT",           LM4_GPIO_H, (1<<3), GPIO_INT_BOTH,
	 extpower_interrupt},
	{"PCH_BKLTEN",           LM4_GPIO_M, (1<<3), GPIO_INT_BOTH,
	 switch_interrupt},
	{"PCH_SLP_S0_L",         LM4_GPIO_G, (1<<6), GPIO_INT_BOTH,
	 x86_power_interrupt},
	{"PCH_SLP_S3_L",         LM4_GPIO_G, (1<<7), GPIO_INT_BOTH,
	 x86_power_interrupt},
	{"PCH_SLP_S5_L",         LM4_GPIO_H, (1<<1), GPIO_INT_BOTH,
	 x86_power_interrupt},
	{"PCH_SLP_SUS_L",        LM4_GPIO_G, (1<<3), GPIO_INT_BOTH,
	 x86_power_interrupt},
	{"PCH_SUSWARN_L",        LM4_GPIO_G, (1<<2), GPIO_INT_BOTH,
	 x86_power_interrupt},
	{"PP1050_PGOOD",         LM4_GPIO_H, (1<<4), GPIO_INT_BOTH,
	 x86_power_interrupt},
	{"PP1350_PGOOD",         LM4_GPIO_H, (1<<6), GPIO_INT_BOTH,
	 x86_power_interrupt},
	{"PP5000_PGOOD",         LM4_GPIO_N, (1<<0), GPIO_INT_BOTH,
	 x86_power_interrupt},
	{"VCORE_PGOOD",          LM4_GPIO_C, (1<<6), GPIO_INT_BOTH,
	 x86_power_interrupt},
	{"PCH_EDP_VDD_EN",       LM4_GPIO_J, (1<<1), GPIO_INT_BOTH,
	 x86_power_interrupt},
	{"RECOVERY_L",           LM4_GPIO_A, (1<<5), GPIO_PULL_UP|GPIO_INT_BOTH,
	 switch_interrupt},
	{"WP",                   LM4_GPIO_A, (1<<4), GPIO_INT_BOTH,
	 switch_interrupt},

	/* Other inputs */
	{"FAN_ALERT_L",          LM4_GPIO_B, (1<<0), GPIO_INPUT, NULL},
	{"USB1_OC_L",            LM4_GPIO_E, (1<<7), GPIO_INPUT, NULL},
	{"USB2_OC_L",            LM4_GPIO_E, (1<<0), GPIO_INPUT, NULL},
	{"BOARD_VERSION1",       LM4_GPIO_Q, (1<<5), GPIO_INPUT, NULL},
	{"BOARD_VERSION2",       LM4_GPIO_Q, (1<<6), GPIO_INPUT, NULL},
	{"BOARD_VERSION3",       LM4_GPIO_Q, (1<<7), GPIO_INPUT, NULL},
	{"CPU_PGOOD",            LM4_GPIO_C, (1<<4), GPIO_INPUT, NULL},

	/* Outputs; all unasserted by default except for reset signals */
	{"CPU_PROCHOT",          LM4_GPIO_B, (1<<1), GPIO_OUT_LOW, NULL},
	{"PP1350_EN",            LM4_GPIO_H, (1<<5), GPIO_OUT_LOW, NULL},
	{"PP3300_DSW_GATED_EN",  LM4_GPIO_J, (1<<3), GPIO_OUT_LOW, NULL},
	{"PP3300_DX_EN",         LM4_GPIO_J, (1<<2), GPIO_OUT_LOW, NULL},
	{"PP3300_LTE_EN",        LM4_GPIO_D, (1<<2), GPIO_OUT_LOW, NULL},
	{"PP3300_WLAN_EN",       LM4_GPIO_J, (1<<0), GPIO_OUT_LOW, NULL},
	{"SUSP_VR_EN",           LM4_GPIO_C, (1<<7), GPIO_OUT_LOW, NULL},
	{"VCORE_EN",             LM4_GPIO_C, (1<<5), GPIO_OUT_LOW, NULL},
	{"PP5000_EN",            LM4_GPIO_H, (1<<7), GPIO_OUT_LOW, NULL},
	{"SYS_PWROK",            LM4_GPIO_H, (1<<2), GPIO_OUT_LOW, NULL},
	{"WLAN_OFF_L",           LM4_GPIO_J, (1<<4), GPIO_OUT_LOW, NULL},
	{"CHARGE_L",             LM4_GPIO_E, (1<<6), GPIO_OUT_LOW, NULL},

	{"ENABLE_BACKLIGHT",     LM4_GPIO_M, (1<<7), GPIO_OUT_LOW, NULL},
	{"ENABLE_TOUCHPAD",      LM4_GPIO_N, (1<<1), GPIO_OUT_LOW, NULL},
	{"ENTERING_RW",          LM4_GPIO_D, (1<<3), GPIO_OUT_LOW, NULL},
	{"PCH_DPWROK",           LM4_GPIO_G, (1<<0), GPIO_OUT_LOW, NULL},
	/*
	 * HDA_SDO is technically an output, but we need to leave it as an
	 * input until we drive it high.  So can't use open-drain (HI_Z).
	 */
	{"PCH_HDA_SDO",          LM4_GPIO_G, (1<<1), GPIO_INPUT, NULL},
	{"PCH_WAKE_L",           LM4_GPIO_F, (1<<0), GPIO_OUT_HIGH, NULL},
	{"PCH_NMI_L",            LM4_GPIO_F, (1<<2), GPIO_OUT_HIGH, NULL},
	{"PCH_PWRBTN_L",         LM4_GPIO_H, (1<<0), GPIO_OUT_HIGH, NULL},
	{"PCH_PWROK",            LM4_GPIO_F, (1<<5), GPIO_OUT_LOW, NULL},
	{"PCH_RCIN_L",           LM4_GPIO_L, (1<<6), GPIO_OUT_HIGH, NULL},
	{"PCH_RSMRST_L",         LM4_GPIO_F, (1<<1), GPIO_OUT_LOW, NULL},
	{"PCH_SMI_L",            LM4_GPIO_F, (1<<4), GPIO_OUT_HIGH, NULL},
	{"TOUCHSCREEN_RESET_L",  LM4_GPIO_N, (1<<7), GPIO_OUT_LOW, NULL},
	{"EC_EDP_VDD_EN",        LM4_GPIO_J, (1<<5), GPIO_OUT_LOW, NULL},

	{"LPC_CLKRUN_L",         LM4_GPIO_M, (1<<2), GPIO_OUT_HIGH, NULL},
	{"USB1_ENABLE",          LM4_GPIO_E, (1<<4), GPIO_OUT_LOW, NULL},
	{"USB2_ENABLE",          LM4_GPIO_D, (1<<5), GPIO_OUT_LOW, NULL},

	{"PCH_SUSACK_L",         LM4_GPIO_F, (1<<3), GPIO_OUT_HIGH, NULL},
	{"PCH_RTCRST_L",         LM4_GPIO_F, (1<<6), GPIO_HI_Z, NULL},
	{"PCH_SRTCRST_L",        LM4_GPIO_F, (1<<7), GPIO_HI_Z, NULL},
};

/* ADC channels. Must be in the exactly same order as in enum adc_channel. */
const struct adc_t adc_channels[ADC_CH_COUNT] = {
	/* EC internal temperature is calculated by
	 * 273 + (295 - 450 * ADC_VALUE / ADC_READ_MAX) / 2
	 * = -225 * ADC_VALUE / ADC_READ_MAX + 420.5
	 */
	{"ECTemp", LM4_ADC_SEQ0, -225, ADC_READ_MAX, 420,
	 LM4_AIN_NONE, 0x0e /* TS0 | IE0 | END0 */, 0, 0},

	/* HEY: need different equation for Falco */
	/* Charger current is mapped from 0~4000mA to 0~1.6V.
	 * And ADC maps 0~3.3V to ADC_READ_MAX.
	 */
	{"ChargerCurrent", LM4_ADC_SEQ1, 33 * 4000, ADC_READ_MAX * 16, 0,
	 LM4_AIN(0), 0x06 /* IE0 | END0 */, LM4_GPIO_E, (1<<3)},
};

/* I2C ports */
const struct i2c_port_t i2c_ports[I2C_PORTS_USED] = {
	/* Note: battery and charger share a port.  Only include it once in
	 * this list so we don't double-initialize it. */
	{"batt_chg", I2C_PORT_BATTERY,  100},
	{"thermal",  I2C_PORT_THERMAL,  100},
};


/* Temperature sensors data; must be in same order as enum temp_sensor_id. */
const struct temp_sensor_t temp_sensors[TEMP_SENSOR_COUNT] = {
/* HEY: Need correct I2C addresses and read function for external sensor */
	{"ECInternal", TEMP_SENSOR_TYPE_BOARD, chip_temp_sensor_get_val, 0, 4},
#ifdef CONFIG_PECI
	{"PECI", TEMP_SENSOR_TYPE_CPU, peci_temp_sensor_get_val, 0, 2},
#endif
};

struct keyboard_scan_config keyscan_config = {
	.output_settle_us = 40,
	.debounce_down_us = 6 * MSEC,
	.debounce_up_us = 30 * MSEC,
	.scan_period_us = 1500,
	.min_post_scan_delay_us = 1000,
	.poll_timeout_us = SECOND,
	.actual_key_mask = {
		0x14, 0xff, 0xff, 0xff, 0xff, 0xf5, 0xff,
		0xa4, 0xff, 0xf6, 0x55, 0xfa, 0xc8  /* full set */
	},
};

/**
 * Configure the GPIOs for the pwm module.
 */
void configure_fan_gpios(void)
{
	/* PN2:3 alternate function 1 = channel 0 PWM/tach */
	gpio_set_alternate_function(LM4_GPIO_N, 0x0c, 1);
}

/**
 * Set wireless switch state.
 */
void board_enable_wireless(uint8_t enabled)
{
	gpio_set_level(GPIO_WLAN_OFF_L,
		       enabled & EC_WIRELESS_SWITCH_WLAN);
	gpio_set_level(GPIO_PP3300_LTE_EN,
		       enabled & EC_WIRELESS_SWITCH_WWAN);
}