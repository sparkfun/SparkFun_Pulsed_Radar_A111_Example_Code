// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "acc_detector_distance_peak.h"
#include "acc_driver_hal.h"
#include "acc_rss.h"
#include "acc_sweep_configuration.h"
#include "acc_version.h"

/**
 * @brief Example that shows how to use the distance peak detector
 *
 * This is an example on how the distance peak detector can be used.
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create a distance peak detector configuration (with blocking mode as default)
 *   - Create a distance peak detector using the previously created configuration
 *   - Activate the distance peak detector
 *   - Get the result and print it 100 times
 *   - Deactivate and destroy the distance peak detector
 *   - Destroy the distance peak detector configuration
 *   - Deactivate Radar System Software
 */


#define FIXED_THRESHOLD_VALUE (500)
#define SENSOR_ID             (1)
#define RANGE_START_M         (0.2f)
#define RANGE_LENGTH_M        (0.5f)
#define ITERATIONS            (20)

static acc_detector_distance_peak_status_t create_detector(acc_detector_distance_peak_handle_t *handle);


static acc_detector_distance_peak_status_t detect_with_blocking_calls(
	acc_detector_distance_peak_handle_t handle);


static char *format_distances(uint16_t                                      reflection_count,
                              const acc_detector_distance_peak_reflection_t *reflections);


static void configure_detector(acc_detector_distance_peak_configuration_t distance_configuration);


int main(void)
{
	acc_detector_distance_peak_status_t detector_status;
	acc_detector_distance_peak_handle_t handle = NULL;

	if (!acc_driver_hal_init())
	{
		return EXIT_FAILURE;
	}

	printf("Acconeer software version %s\n", ACC_VERSION);
	printf("Acconeer RSS version %s\n", acc_rss_version());

	acc_hal_t hal = acc_driver_hal_get_implementation();

	if (!acc_rss_activate_with_hal(&hal))
	{
		return EXIT_FAILURE;
	}

	// Create the detector
	create_detector(&handle);

	// Run distance peak detection in blocking mode
	detector_status = detect_with_blocking_calls(handle);
	if (detector_status != ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS)
	{
		fprintf(stderr, "Running distance peak detector in blocking mode failed\n");
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_detector_distance_peak_destroy(&handle);

	acc_rss_deactivate();

	return EXIT_SUCCESS;
}


acc_detector_distance_peak_status_t create_detector(acc_detector_distance_peak_handle_t *handle)
{
	acc_detector_distance_peak_configuration_t distance_configuration;
	acc_detector_distance_peak_status_t        detector_status;

	if (handle == NULL)
	{
		fprintf(stderr, "handle is NULL\n");
		return ACC_DETECTOR_DISTANCE_PEAK_STATUS_FAILURE;
	}

	distance_configuration = acc_detector_distance_peak_configuration_create();

	if (distance_configuration == NULL)
	{
		fprintf(stderr, "Create configuration failed\n");
		return ACC_DETECTOR_DISTANCE_PEAK_STATUS_FAILURE;
	}

	// Return reflections with amplitude above threshold
	detector_status = acc_detector_distance_peak_set_threshold_mode_fixed(distance_configuration, FIXED_THRESHOLD_VALUE);
	if (detector_status != ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS)
	{
		fprintf(stderr, "Setting threshold cancellation mode failed.\n");
		return detector_status;
	}

	detector_status = acc_detector_distance_peak_set_absolute_amplitude(distance_configuration, true);
	if (detector_status != ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS)
	{
		fprintf(stderr, "Set absolute amplitude failed.\n");
		return detector_status;
	}

	detector_status = acc_detector_distance_peak_set_sort_by_amplitude(distance_configuration, true);
	if (detector_status != ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS)
	{
		fprintf(stderr, "Sort by amplitude failed.\n");
		return detector_status;
	}

	configure_detector(distance_configuration);

	*handle = acc_detector_distance_peak_create(distance_configuration);

	if (*handle == NULL)
	{
		fprintf(stderr, "Create detector failed\n");
		return ACC_DETECTOR_DISTANCE_PEAK_STATUS_FAILURE;
	}

	acc_detector_distance_peak_configuration_destroy(&distance_configuration);

	return ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS;
}


acc_detector_distance_peak_status_t detect_with_blocking_calls(acc_detector_distance_peak_handle_t handle)
{
	acc_detector_distance_peak_status_t     detector_status = ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS;
	acc_detector_distance_peak_metadata_t   metadata;
	uint_fast16_t                           reflection_count = 10;
	acc_detector_distance_peak_reflection_t reflections[reflection_count];

	if (handle == NULL)
	{
		fprintf(stderr, "handle == NULL\n");
		return ACC_DETECTOR_DISTANCE_PEAK_STATUS_FAILURE;
	}

	printf("Running distance peak detector in blocking mode\n");

	acc_detector_distance_peak_get_metadata(handle, &metadata);
	printf("Actual start: %u mm\n", (unsigned int)(metadata.actual_start_m * 1000.0f));
	printf("Actual length: %u mm\n", (unsigned int)(metadata.actual_length_m * 1000.0f));
	printf("Actual end: %u mm\n", (unsigned int)((metadata.actual_start_m + metadata.actual_length_m) * 1000.0f));
	printf("\n");

	acc_detector_distance_peak_result_info_t result_info;

	detector_status = acc_detector_distance_peak_activate(handle);

	if (detector_status != ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS)
	{
		fprintf(stderr, "acc_detector_distance_peak_activate failed\n");
		return ACC_DETECTOR_DISTANCE_PEAK_STATUS_FAILURE;
	}

	for (int i = 0; i < ITERATIONS; i++)
	{
		reflection_count = 10;

		detector_status = acc_detector_distance_peak_get_next(handle,
		                                                      reflections,
		                                                      &reflection_count,
		                                                      &result_info);

		if (detector_status == ACC_DETECTOR_DISTANCE_PEAK_STATUS_SUCCESS)
		{
			float start_m = metadata.actual_start_m;
			float end_m   = metadata.actual_start_m + metadata.actual_length_m;

			printf("Distance detector: Reflections: %u. Seq. nr: %u. Data saturated: %s.  (%u-%u mm): %s\n",
			       (unsigned int)reflection_count,
			       (unsigned int)result_info.sequence_number,
			       result_info.data_saturated ? "true" : "false",
			       (unsigned int)(start_m * 1000.0f),
			       (unsigned int)(end_m * 1000.0f),
			       format_distances(reflection_count, reflections));

			format_distances(reflection_count, reflections);
		}
		else
		{
			fprintf(stderr, "Reflection data not properly retrieved\n");
			return detector_status;
		}
	}

	detector_status = acc_detector_distance_peak_deactivate(handle);

	return detector_status;
}


char *format_distances(uint16_t                                      reflection_count,
                       const acc_detector_distance_peak_reflection_t *reflections)
{
	static char buffer[1024];
	size_t      total_count = 0;
	int         count;

	*buffer = 0;
	for (uint_fast8_t reflection_index = 0; reflection_index < reflection_count; reflection_index++)
	{
		if (total_count > 0)
		{
			count = snprintf(&buffer[total_count], sizeof(buffer) - total_count, ", ");
			if (count < 0)
			{
				break;
			}

			total_count += count;
		}

		count = snprintf(&buffer[total_count], sizeof(buffer) - total_count, "%u mm (%u)",
		                 (unsigned int)((reflections[reflection_index].distance) * 1000.0f),
		                 (unsigned int)(reflections[reflection_index].amplitude));
		if (count < 0)
		{
			break;
		}

		total_count += count;

		if (total_count >= sizeof(buffer) - 1)
		{
			break;
		}
	}

	return buffer;
}


void configure_detector(acc_detector_distance_peak_configuration_t distance_configuration)
{
	acc_sweep_configuration_t sweep_configuration = acc_detector_distance_peak_get_sweep_configuration(distance_configuration);

	if (sweep_configuration == NULL)
	{
		fprintf(stderr, "Sweep configuration not available\n");
	}
	else
	{
		acc_detector_distance_peak_profile_set(distance_configuration, ACC_DETECTOR_DISTANCE_PEAK_PROFILE_MAXIMIZE_DEPTH_RESOLUTION);

		acc_sweep_configuration_sensor_set(sweep_configuration, SENSOR_ID);
		acc_sweep_configuration_requested_start_set(sweep_configuration, RANGE_START_M);
		acc_sweep_configuration_requested_length_set(sweep_configuration, RANGE_LENGTH_M);
	}
}
