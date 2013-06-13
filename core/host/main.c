/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Entry point of unit test executable */

#include "flash.h"
#include "hooks.h"
#include "task.h"
#include "timer.h"
#include "uart.h"

int main(void)
{
	flash_pre_init();

	timer_init();

	hook_init();

	uart_init();

	task_start();

	return 0;
}