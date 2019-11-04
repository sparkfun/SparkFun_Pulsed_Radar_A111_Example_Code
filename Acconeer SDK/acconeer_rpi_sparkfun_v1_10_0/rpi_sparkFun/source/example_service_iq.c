// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#include <complex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_driver_hal.h"
#include "acc_rss.h"
#include "acc_service.h"
#include "acc_service_iq.h"
#include "acc_sweep_configuration.h"

#include "acc_version.h"


/**
 * @brief Example that shows how to use the IQ service
 *
 * This is an example on how the IQ service can be used.
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create an IQ service configuration (with blocking mode as default)
 *   - Create an IQ service using the previously created configuration
 *   - Activate the IQ service
 *   - Get the result and print it 5 times, where the last result is intentionally late
 *   - Deactivate and destroy the IQ service
 *   - Create and activate IQ service
 *   - Destroy the IQ service configuration
 *   - Deactivate Radar System Software
 */


static acc_service_status_t execute_iq_with_blocking_calls(acc_service_configuration_t iq_configuration);


static void configure_sweeps(acc_service_configuration_t iq_configuration);


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

	acc_service_configuration_t iq_configuration = acc_service_iq_configuration_create();

	if (iq_configuration == NULL)
	{
		printf("acc_service_iq_configuration_create() failed\n");
		return EXIT_FAILURE;
	}

	configure_sweeps(iq_configuration);

	acc_service_status_t service_status;

	service_status = execute_iq_with_blocking_calls(iq_configuration);

	if (service_status != ACC_SERVICE_STATUS_OK)
	{
		printf("execute_iq_with_blocking_calls() => (%u) %s\n", (unsigned int)service_status, acc_service_status_name_get(service_status));
		return EXIT_FAILURE;
	}

	acc_service_iq_configuration_destroy(&iq_configuration);

	acc_rss_deactivate();

	return EXIT_SUCCESS;
}


acc_service_status_t execute_iq_with_blocking_calls(acc_service_configuration_t iq_configuration)
{
	acc_service_handle_t handle = acc_service_create(iq_configuration);

	if (handle == NULL)
	{
		printf("acc_service_create failed\n");
		return ACC_SERVICE_STATUS_FAILURE_UNSPECIFIED;
	}

	acc_service_iq_metadata_t iq_metadata;
	acc_service_iq_get_metadata(handle, &iq_metadata);

	printf("Actual start: %u mm\n", (unsigned int)(iq_metadata.actual_start_m * 1000.0f));
	printf("Actual length: %u mm\n", (unsigned int)(iq_metadata.actual_length_m * 1000.0f));
	printf("Actual end: %u mm\n", (unsigned int)((iq_metadata.actual_start_m + iq_metadata.actual_length_m) * 1000.0f));
	printf("Data length: %u\n", (unsigned int)(iq_metadata.data_length));

	float complex                data[iq_metadata.data_length];
	acc_service_iq_result_info_t result_info;

	acc_service_status_t service_status = acc_service_activate(handle);

	if (service_status == ACC_SERVICE_STATUS_OK)
	{
		uint_fast8_t sweep_count = 5;

		while (sweep_count > 0)
		{
			service_status = acc_service_iq_get_next(handle, data, iq_metadata.data_length, &result_info);

			if (service_status == ACC_SERVICE_STATUS_OK)
			{
				printf("IQ result_info->sequence_number: %u\n", (unsigned int)result_info.sequence_number);
				printf("IQ data in polar coordinates (r, phi) (multiplied by 1000):\n");
				for (uint_fast16_t index = 0; index < iq_metadata.data_length; index++)
				{
					if (index && !(index % 8))
					{
						printf("\n");
					}

					printf("(%d, %d)\t", (int)(cabsf(data[index]) * 1000), (int)(cargf(data[index]) * 1000));
				}

				printf("\n");
			}
			else
			{
				printf("IQ data not properly retrieved\n");
			}

			sweep_count--;

			// Show what happens if application is late
			if (sweep_count == 1)
			{
				hal.os.sleep_us(200000);
			}
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


void configure_sweeps(acc_service_configuration_t iq_configuration)
{
	acc_sweep_configuration_t sweep_configuration = acc_service_get_sweep_configuration(iq_configuration);

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
