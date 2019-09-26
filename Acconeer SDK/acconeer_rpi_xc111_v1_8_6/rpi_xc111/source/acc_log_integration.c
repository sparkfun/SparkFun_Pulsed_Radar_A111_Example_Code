// Copyright (c) Acconeer AB, 2019
// All rights reserved

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "acc_log_integration.h"

#include "acc_definitions.h"
#include "acc_device_os.h"


#define LOG_FORMAT          "%02u:%02u:%02u.%03u [%5u] (%c) (%s): %s\n"


static acc_log_level_t log_level_limit = ACC_LOG_LEVEL_DIAGNOSTICS;


void acc_log_integration(acc_log_level_t level, const char *module, const char *buffer)
{
	acc_os_thread_id_t thread_id;
	uint32_t           time_usec = 0;
	char               level_ch;

	if (level > log_level_limit)
	{
		return;
	}

	thread_id = acc_os_get_thread_id();

	acc_os_get_time(&time_usec);

	unsigned int timestamp    = time_usec;
	unsigned int hours        = timestamp / 1000 / 1000 / 60 / 60;
	unsigned int minutes      = timestamp / 1000 / 1000 / 60 % 60;
	unsigned int seconds      = timestamp / 1000 / 1000 % 60;
	unsigned int milliseconds = timestamp / 1000 % 1000;

	level_ch = (level < ACC_LOG_LEVEL_MAX) ? "EWIVDD"[level] : '?';

	printf(LOG_FORMAT, hours, minutes, seconds, milliseconds, (unsigned int)thread_id, level_ch, module, buffer);

	fflush(stdout);
}
