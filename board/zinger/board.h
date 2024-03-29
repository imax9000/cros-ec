/* Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Tiny charger configuration */

#ifndef __BOARD_H
#define __BOARD_H

/* 48 MHz SYSCLK clock frequency */
#define CPU_CLOCK 48000000

/* the UART console is on USART1 (PA9/PA10) */
#define CONFIG_UART_CONSOLE 1

/* Optional features */
#define CONFIG_USB_POWER_DELIVERY
#undef CONFIG_USB_PD_DUAL_ROLE
#undef CONFIG_USB_PD_INTERNAL_COMP
#define CONFIG_USB_PD_CUSTOM_VDM
#define CONFIG_HW_CRC
#define CONFIG_SHA1
#undef CONFIG_WATCHDOG_HELP
#undef CONFIG_LID_SWITCH
#undef CONFIG_TASK_PROFILING
#undef CONFIG_COMMON_GPIO
#undef CONFIG_COMMON_PANIC_OUTPUT
#undef CONFIG_COMMON_RUNTIME
#undef CONFIG_COMMON_TIMER
#undef CONFIG_CONSOLE_CMDHELP
#undef CONFIG_DEBUG_ASSERT
#undef CONFIG_DEBUG_EXCEPTIONS
#undef CONFIG_DEBUG_STACK_OVERFLOW
#undef CONFIG_FLASH
#undef CONFIG_FMAP

/* Stub value */
#define TASK_ID_PD 0

/* debug printf flash footprinf is about 1400 bytes */
#define CONFIG_DEBUG_PRINTF
#define UARTN CONFIG_UART_CONSOLE

#ifndef __ASSEMBLER__

#include "common.h"

/* No GPIO abstraction layer */
enum gpio_signal;

enum adc_channel {
	ADC_CH_CC1_PD = 1,
	ADC_CH_A_SENSE = 2,
	ADC_CH_V_SENSE = 3,
	/* Number of ADC channels */
	ADC_CH_COUNT
};
/* captive cable : no CC2 */
#define ADC_CH_CC2_PD ADC_CH_CC1_PD

/* Initialize all useful registers */
void hardware_init(void);

/* last interrupt event */
extern volatile uint32_t last_event;

/* RW section flashing */
int flash_erase_rw(void);
int flash_write_rw(int offset, int size, const char *data);
uint8_t *flash_hash_rw(void);
int is_ro_mode(void);

/* Reboot the CPU */
void cpu_reset(void);

#endif /* !__ASSEMBLER__ */

#endif /* __BOARD_H */
