// Copyright (c) Acconeer AB, 2019
// All rights reserved

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_driver_hal.h"
#include "acc_rss.h"
#include "acc_service.h"
#include "acc_service_sparse.h"
#include "acc_sweep_configuration.h"

#include "acc_version.h"


/**
 * @brief Example that shows how to use the sparse service
 *
 * This is an example on how the sparse service can be used.
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create a sparse service configuration (with blocking mode as default)
 *   - Create a sparse service using the previously created configuration
 *   - Activate the sparse service
 *   - Get the result and print it 5 times, where the last result is intentionally late
 *   - Create and activate sparse service
 *   - Destroy the sparse service configuration
 *   - Deactivate Radar System Software
 */


static acc_service_status_t execute_sparse_with_blocking_calls(acc_service_configuration_t sparse_configuration);


static void configure_sweeps(acc_service_configuration_t sparse_configuration);


static acc_hal_t hal;


static void print_data(const char *header, const uint16_t *data, int length)
{
	printf("%s", header);

	for (int index = 0; index < length; index++)
	{
		if (index && !(index % 8))
		{
			printf("\n");
		}

		printf("%6u,", (unsigned int)(data[index]));
	}

	printf("\n");
}


static void print_metadata(acc_service_sparse_metadata_t *metadata)
{
	printf("Actual start: %u mm\n", (unsigned int)(metadata->actual_start_m * 1000.0f));
	printf("Actual length: %u mm\n", (unsigned int)(metadata->actual_length_m * 1000.0f));
	printf("Actual end: %u mm\n", (unsigned int)((metadata->actual_start_m + metadata->actual_length_m) * 1000.0f));
	printf("Data length: %u\n", (unsigned int)(metadata->data_length));
}


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

	acc_service_configuration_t configuration = acc_service_sparse_configuration_create();

	if (configuration == NULL)
	{
		printf("acc_service_sparse_configuration_create() failed\n");
		return EXIT_FAILURE;
	}

	configure_sweeps(configuration);

	acc_service_status_t service_status;

	service_status = execute_sparse_with_blocking_calls(configuration);

	if (service_status != ACC_SERVICE_STATUS_OK)
	{
		printf("execute_sparse_with_blocking_calls() => (%u) %s\n", (unsigned int)service_status,
		       acc_service_status_name_get(service_status));
		acc_service_sparse_configuration_destroy(&configuration);
		return EXIT_FAILURE;
	}

	acc_service_sparse_configuration_destroy(&configuration);

	acc_rss_deactivate();

	return EXIT_SUCCESS;
}


acc_service_status_t execute_sparse_with_blocking_calls(acc_service_configuration_t configuration)
{
	acc_service_handle_t handle = acc_service_create(configuration);

	if (handle == NULL)
	{
		printf("acc_service_create failed\n");
		return ACC_SERVICE_STATUS_FAILURE_UNSPECIFIED;
	}

	acc_service_sparse_metadata_t metadata;
	acc_service_sparse_get_metadata(handle, &metadata);
	print_metadata(&metadata);

	acc_service_sparse_result_info_t result_info;
	acc_service_status_t             service_status = acc_service_activate(handle);

	if (service_status == ACC_SERVICE_STATUS_OK)
	{
		uint16_t data[metadata.data_length];
		int      sweep_count = 5;

		while (sweep_count > 0)
		{
			service_status = acc_service_sparse_get_next(handle, data, metadata.data_length, &result_info);

			if (result_info.sensor_communication_error)
			{
				// Handle error, for example restart service
			}

			if (result_info.data_saturated)
			{
				// Handle warning, for example lowering the gain
			}

			if (service_status == ACC_SERVICE_STATUS_OK)
			{
				printf("Sparse result_info.sequence_number: %u\n", (unsigned int)result_info.sequence_number);
				print_data("sparse_data:\n", data, metadata.data_length);
			}
			else
			{
				printf("Sparse data not properly retrieved, service_status:%u\n", (unsigned int)service_status);
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


void configure_sweeps(acc_service_configuration_t configuration)
{
	acc_sweep_configuration_t sweep_configuration = acc_service_get_sweep_configuration(configuration);

	if (configuration == NULL)
	{
		printf("Sweep configuration not available\n");
	}
	else
	{
		float start_m        = 0.18f;
		float length_m       = 0.36f;
		float update_rate_hz = 100;

		acc_sweep_configuration_requested_start_set(sweep_configuration, start_m);
		acc_sweep_configuration_requested_length_set(sweep_configuration, length_m);

		acc_sweep_configuration_repetition_mode_streaming_set(sweep_configuration, update_rate_hz);
	}
}
