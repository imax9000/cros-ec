/* Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Keyborg board configuration */

#ifndef __BOARD_H
#define __BOARD_H

/* 48 MHz SYSCLK clock frequency */
#define CPU_CLOCK 48000000

/* Optional features */
#define CONFIG_DEBUG_PRINTF
#undef CONFIG_ADC
#undef CONFIG_COMMON_GPIO
#undef CONFIG_COMMON_PANIC_OUTPUT
#undef CONFIG_COMMON_RUNTIME
#undef CONFIG_COMMON_TIMER
#undef CONFIG_CONSOLE_CMDHELP
#undef CONFIG_DEBUG_EXCEPTIONS
#undef CONFIG_DEBUG_STACK_OVERFLOW
#undef CONFIG_FLASH
#undef CONFIG_FMAP
#undef CONFIG_LID_SWITCH
#undef CONFIG_TASK_PROFILING
#undef CONFIG_WATCHDOG_HELP

#ifndef __ASSEMBLER__

enum gpio_signal;

/* Initialize all useful registers */
void hardware_init(void);

#endif /* !__ASSEMBLER__ */

#endif /* __BOARD_H */
