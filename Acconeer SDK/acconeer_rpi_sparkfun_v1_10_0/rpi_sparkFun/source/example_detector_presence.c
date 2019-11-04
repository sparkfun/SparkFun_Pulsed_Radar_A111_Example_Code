// Copyright (c) Acconeer AB, 2019
// All rights reserved

#include <stdbool.h>
#include <stdio.h>

#include "acc_definitions.h"
#include "acc_detector_presence.h"
#include "acc_driver_hal.h"
#include "acc_rss.h"
#include "acc_sweep_configuration.h"
#include "acc_version.h"


static acc_hal_t hal;

#define DEFAULT_START_M             (0.2f)
#define DEFAULT_LENGTH_M            (1.4f)
#define DEFAULT_UPDATE_RATE         (10)
#define DEFAULT_POWER_SAVE_MODE     ACC_SWEEP_CONFIGURATION_POWER_SAVE_MODE_B
#define DEFAULT_DETECTION_THRESHOLD (2.0f)


static void set_default_configuration(acc_detector_presence_configuration_t presence_configuration)
{
	acc_detector_presence_configuration_update_rate_set(presence_configuration, DEFAULT_UPDATE_RATE);
	acc_detector_presence_configuration_detection_threshold_set(presence_configuration, DEFAULT_DETECTION_THRESHOLD);
	acc_detector_presence_configuration_range_start_set(presence_configuration, DEFAULT_START_M);
	acc_detector_presence_configuration_range_length_set(presence_configuration, DEFAULT_LENGTH_M);
	acc_detector_presence_configuration_power_save_mode_set(presence_configuration, DEFAULT_POWER_SAVE_MODE);
}


int main(void)
{
	if (!acc_driver_hal_init())
	{
		return EXIT_FAILURE;
	}

	printf("Acconeer software version %s\n", ACC_VERSION);
	printf("Acconeer RSS version %s\n", acc_rss_version());

	hal = acc_driver_hal_get_implementation();

	if (!acc_rss_activate_with_hal(&hal))
	{
		fprintf(stderr, "Failed to activate RSS\n");
		return EXIT_FAILURE;
	}

	acc_detector_presence_configuration_t presence_configuration = acc_detector_presence_configuration_create();
	if (presence_configuration == NULL)
	{
		fprintf(stderr, "Failed to create configuration\n");
		return EXIT_FAILURE;
	}

	set_default_configuration(presence_configuration);

	acc_detector_presence_handle_t handle = acc_detector_presence_create(presence_configuration);
	if (handle == NULL)
	{
		fprintf(stderr, "Failed to create detector\n");
		return EXIT_FAILURE;
	}

	if (!acc_detector_presence_activate(handle))
	{
		fprintf(stderr, "Failed to activate detector\n");
		return EXIT_FAILURE;
	}

	acc_detector_presence_result_t result;

	for (int i = 0; i < 200; i++)
	{
		acc_detector_presence_get_next(handle, &result);

		if (result.presence_detected)
		{
			printf("Motion\n");
		}
		else
		{
			printf("No motion\n");
		}

		printf("Presence score: %d, Distance: %d\n", (int)(result.presence_score * 1000.0f), (int)(result.presence_distance * 1000.0f));
	}

	acc_detector_presence_deactivate(handle);

	acc_detector_presence_destroy(&handle);

	acc_detector_presence_configuration_destroy(&presence_configuration);

	return EXIT_SUCCESS;
}
