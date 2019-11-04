// Copyright (c) Acconeer AB, 2015-2019
// All rights reserved

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_driver_hal.h"
#include "acc_rss.h"
#include "acc_service.h"
#include "acc_service_power_bins.h"
#include "acc_sweep_configuration.h"

#include "acc_version.h"


/**
 * @brief Example that shows how to use the Power Bins service
 *
 * This is an example on how the Power Bins service can be used.
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create a Power Bins service configuration (with blocking mode as default)
 *   - Create a Power Bins service using the previously created configuration
 *   - Activate the Power Bins service
 *   - Get the result and print it 5 times, where the last result is intentionally late
 *   - Deactivate and destroy the Power Bins service
 *   - Create and activate Power Bins service
 *   - Destroy the Power Bins service configuration
 *   - Deactivate Radar System Software
 */


static acc_service_status_t execute_power_bins_with_blocking_calls(acc_service_configuration_t power_bins_configuration);


static void configure_sweeps(acc_service_configuration_t power_bins_configuration);


static acc_hal_t hal;


int main(void)
{
	if (!acc_driver_hal_init())
	{
		return EXIT_FAILURE;
	}

	printf("Acconeer software version %s\n", ACC_VERSION);

	hal = acc_driver_hal_get_implementation();

	if (!acc_rss_activate_with_hal(&hal))
	{
		return EXIT_FAILURE;
	}

	acc_service_configuration_t power_bins_configuration = acc_service_power_bins_configuration_create();

	if (power_bins_configuration == NULL)
	{
		printf("acc_service_power_bins_configuration_create() failed\n");
		return EXIT_FAILURE;
	}

	configure_sweeps(power_bins_configuration);

	acc_service_status_t service_status;

	service_status = execute_power_bins_with_blocking_calls(power_bins_configuration);

	if (service_status != ACC_SERVICE_STATUS_OK)
	{
		printf("execute_power_bins_with_blocking_calls() => (%u) %s\n", (unsigned int)service_status,
		       acc_service_status_name_get(service_status));
		acc_service_power_bins_configuration_destroy(&power_bins_configuration);
		return EXIT_FAILURE;
	}

	acc_service_power_bins_configuration_destroy(&power_bins_configuration);

	acc_rss_deactivate();

	return EXIT_SUCCESS;
}


acc_service_status_t execute_power_bins_with_blocking_calls(acc_service_configuration_t power_bins_configuration)
{
	acc_service_handle_t handle = acc_service_create(power_bins_configuration);

	if (handle == NULL)
	{
		printf("acc_service_create failed\n");
		return ACC_SERVICE_STATUS_FAILURE_UNSPECIFIED;
	}

	acc_service_power_bins_metadata_t power_bins_metadata;
	acc_service_power_bins_get_metadata(handle, &power_bins_metadata);

	printf("Actual start: %u mm\n", (unsigned int)(power_bins_metadata.actual_start_m * 1000.0f));
	printf("Actual length: %u mm\n", (unsigned int)(power_bins_metadata.actual_length_m * 1000.0f));
	printf("Actual end: %u mm\n", (unsigned int)((power_bins_metadata.actual_start_m + power_bins_metadata.actual_length_m) * 1000.0f));
	printf("Bin count: %u\n", (unsigned int)(power_bins_metadata.actual_bin_count));

	float data[power_bins_metadata.actual_bin_count];

	acc_service_power_bins_result_info_t result_info;
	acc_service_status_t                 service_status = acc_service_activate(handle);

	if (service_status == ACC_SERVICE_STATUS_OK)
	{
		uint_fast8_t sweep_count = 5;

		while (sweep_count > 0)
		{
			service_status = acc_service_power_bins_get_next(handle, data, power_bins_metadata.actual_bin_count, &result_info);

			if (service_status == ACC_SERVICE_STATUS_OK)
			{
				printf("Power_bins result_info.sequence_number: %u\n", (unsigned int)result_info.sequence_number);
				printf("Power_bins data:\n");
				for (uint_fast16_t index = 0; index < power_bins_metadata.actual_bin_count; index++)
				{
					printf("%u\t", (unsigned int)(data[index]));
				}

				printf("\n");
			}
			else
			{
				printf("Power_bins data not properly retrieved\n");
			}

			sweep_count--;
		}

		service_status = acc_service_deactivate(handle);
	}
	else
	{
		printf("acc_service_activate() %u => %s\n", (unsigned int)service_status, acc_service_status_name_get(service_status));
	}

	acc_service_destroy(&handle);

	return service_status;
}


void configure_sweeps(acc_service_configuration_t power_bins_configuration)
{
	acc_sweep_configuration_t sweep_configuration = acc_service_get_sweep_configuration(power_bins_configuration);

	if (sweep_configuration == NULL)
	{
		printf("Sweep configuration not available\n");
	}
	else
	{
		float start_m        = 0.4f;
		float length_m       = 0.5f;
		float update_rate_hz = 100;

		acc_sweep_configuration_requested_start_set(sweep_configuration, start_m);
		acc_sweep_configuration_requested_length_set(sweep_configuration, length_m);

		acc_sweep_configuration_repetition_mode_streaming_set(sweep_configuration, update_rate_hz);
	}
}
