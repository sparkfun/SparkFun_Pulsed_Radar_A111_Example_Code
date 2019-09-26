// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#include <stdbool.h>
#include <stdint.h>

#include "acc_driver_hal.h"

#include "acc_board.h"
#include "acc_definitions.h"
#include "acc_device_spi.h"
#include "acc_driver_os.h"
#include "acc_log_integration.h"


//-----------------------------
// Public definitions
//-----------------------------

bool acc_driver_hal_init(void)
{
	if (!acc_board_init())
	{
		return false;
	}

	if (!acc_board_gpio_init())
	{
		return false;
	}

	return true;
}


acc_hal_t acc_driver_hal_get_implementation(void)
{
	acc_hal_t hal =
	{
		.properties.sensor_count = acc_board_get_sensor_count(),
		.properties.max_spi_transfer_size = acc_device_spi_get_max_transfer_size(),

		.sensor_device.power_on = acc_board_start_sensor,
		.sensor_device.power_off = acc_board_stop_sensor,
		.sensor_device.wait_for_interrupt = acc_board_wait_for_sensor_interrupt,
		.sensor_device.transfer = acc_board_sensor_transfer,
		.sensor_device.get_reference_frequency = acc_board_get_ref_freq,

		.os.sleep_us = acc_device_os_sleep_us_func,
		.os.mem_alloc = acc_device_os_mem_alloc_func,
		.os.mem_free = acc_device_os_mem_free_func,
		.os.get_thread_id = acc_device_os_get_thread_id_func,
		.os.gettime = acc_device_os_get_time_func,
		.os.mutex_create = acc_device_os_mutex_create_func,
		.os.mutex_destroy = acc_device_os_mutex_destroy_func,
		.os.mutex_lock = acc_device_os_mutex_lock_func,
		.os.mutex_unlock = acc_device_os_mutex_unlock_func,
		.os.thread_create = acc_device_os_thread_create_func,
		.os.thread_exit = acc_device_os_thread_exit_func,
		.os.thread_cleanup = acc_device_os_thread_cleanup_func,
		.os.semaphore_create = acc_device_os_semaphore_create_func,
		.os.semaphore_destroy = acc_device_os_semaphore_destroy_func,
		.os.semaphore_wait = acc_device_os_semaphore_wait_func,
		.os.semaphore_signal = acc_device_os_semaphore_signal_func,
		.os.semaphore_signal_from_interrupt = acc_device_os_semaphore_signal_from_interrupt_func,

		.log.log_level = ACC_LOG_LEVEL_INFO,
		.log.log = acc_log_integration
	};

	return hal;
}
