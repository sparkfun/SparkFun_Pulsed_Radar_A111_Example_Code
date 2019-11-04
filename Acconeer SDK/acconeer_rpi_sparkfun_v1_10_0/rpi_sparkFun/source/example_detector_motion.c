// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_definitions.h"
#include "acc_detector_motion.h"
#include "acc_driver_hal.h"
#include "acc_rss.h"
#include "acc_sweep_configuration.h"

#include "acc_version.h"


/**
 * @brief Example that shows how to use the motion detector
 *
 * This is an example of how the motion detector can be used.
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Set log level, optional
 *   - Create a motion detector configuration
 *   - Create a motion detector using the previously created configuration
 *   - Activate the motion detector
 *   - Get the result and print it 5 times
 *   - Deactivate and destroy the motion detector
 *   - Reconfigure the motion detector
 *   - Activate the motion detector
 *   - Get the result and print it 5 times
 *   - Deactivate and destroy the motion detector
 *   - Destroy the motion detector configuration
 *   - Deactivate Radar System Software
 */


static bool execute_motion_detection(acc_detector_motion_configuration_t motion_configuration);


static void reconfigure_detector(acc_detector_motion_configuration_t motion_configuration);


static acc_hal_t hal;


int main(void)
{
	if (!acc_driver_hal_init())
	{
		return EXIT_FAILURE;
	}

	printf("Acconeer software version %s\n", ACC_VERSION);
	printf("Acconeer RSS version %s\n", acc_rss_version());

	hal = acc_driver_hal_get_implementation();

	hal.log.log_level = ACC_LOG_LEVEL_ERROR;

	if (!acc_rss_activate_with_hal(&hal))
	{
		return EXIT_FAILURE;
	}

	acc_detector_motion_configuration_t motion_configuration = acc_detector_motion_configuration_create();

	if (motion_configuration == NULL)
	{
		printf("acc_detector_motion_configuration_create() failed\n");
		return EXIT_FAILURE;
	}

	if (!execute_motion_detection(motion_configuration))
	{
		printf("execute_motion_detection() failed\n");
		acc_detector_motion_configuration_destroy(&motion_configuration);
		return EXIT_FAILURE;
	}

	reconfigure_detector(motion_configuration);

	if (!execute_motion_detection(motion_configuration))
	{
		printf("execute_motion_detection() failed()\n");
		acc_detector_motion_configuration_destroy(&motion_configuration);
		return EXIT_FAILURE;
	}

	acc_detector_motion_configuration_destroy(&motion_configuration);

	acc_rss_deactivate();

	return EXIT_SUCCESS;
}


bool execute_motion_detection(acc_detector_motion_configuration_t motion_configuration)
{
	bool                         success = true;
	acc_detector_motion_handle_t handle  = acc_detector_motion_create(motion_configuration);
	float update_rate                    = acc_detector_motion_configuration_update_rate_get(motion_configuration);

	if (handle != NULL)
	{
		if (acc_detector_motion_activate(handle))
		{
			bool         motion_detect = false;
			uint_fast8_t sweep_count   = 5;

			while (sweep_count > 0)
			{
				uint32_t start_time_us;
				hal.os.gettime(&start_time_us);

				if (acc_detector_motion_get_next(handle, &motion_detect))
				{
					printf("Motion detect: %s\n", motion_detect ? "true" : "false");
				}
				else
				{
					printf("Detector motion data not properly retrieved\n");
					success = false;
					break;
				}

				uint32_t stop_time_us;
				hal.os.gettime(&stop_time_us);

				uint32_t      sleep_us         = 0;
				uint_fast32_t update_period_us = (1.0f / update_rate) * 1000000.0f;
				uint_fast32_t duration_us      = stop_time_us - start_time_us;

				if (update_period_us > duration_us)
				{
					sleep_us = update_period_us - duration_us;
				}

				hal.os.sleep_us(sleep_us);

				sweep_count--;
			}

			if (!acc_detector_motion_deactivate(handle))
			{
				printf("acc_detector_motion_deactivate() failed\n");
				success = false;
			}
		}
		else
		{
			printf("acc_detector_motion_activate() failed\n");
			success = false;
		}
	}
	else
	{
		printf("acc_detector_motion_create() failed\n");
		success = false;
	}

	acc_detector_motion_destroy(&handle);

	return success;
}


void reconfigure_detector(acc_detector_motion_configuration_t motion_configuration)
{
	float update_rate = 1.3f;

	acc_detector_motion_configuration_update_rate_set(motion_configuration, update_rate);

	acc_sweep_configuration_t sweep_configuration = acc_detector_motion_configuration_get_sweep_configuration(motion_configuration);

	if (sweep_configuration == NULL)
	{
		printf("Sweep configuration not available\n");
	}
	else
	{
		float start_m  = 1.0f;
		float length_m = 0.4f;

		acc_sweep_configuration_requested_start_set(sweep_configuration, start_m);
		acc_sweep_configuration_requested_length_set(sweep_configuration, length_m);
	}
}
