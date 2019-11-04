// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "acc_detector_obstacle.h"
#include "acc_driver_hal.h"
#include "acc_rss.h"
#include "acc_version.h"


static bool execute_obstacle_with_blocking_calls(acc_detector_obstacle_configuration_t configuration);


static acc_hal_t hal;

static uint8_t *background_estimation_data = NULL;


int main(void)
{
	bool status;

	if (!acc_driver_hal_init())
	{
		return EXIT_FAILURE;
	}

	printf("Acconeer software version %s\n", ACC_VERSION);
	printf("Acconeer RSS version %s\n", acc_rss_version());

	hal = acc_driver_hal_get_implementation();

	if (!acc_rss_activate_with_hal(&hal))
	{
		printf("Failed to activate RSS.\n");
		return EXIT_FAILURE;
	}

	acc_detector_obstacle_configuration_t configuration = acc_detector_obstacle_configuration_create();
	if (configuration == NULL)
	{
		fprintf(stderr, "Failed to create configuration.\n");
		return EXIT_FAILURE;
	}

	status = execute_obstacle_with_blocking_calls(configuration);

	if (!status)
	{
		printf("execute_obstacle_with_blocking_calls failed\n");
		acc_detector_obstacle_configuration_destroy(&configuration);
		acc_rss_deactivate();
		return EXIT_FAILURE;
	}

	acc_detector_obstacle_configuration_destroy(&configuration);

	hal.os.mem_free(background_estimation_data);
	background_estimation_data = NULL;

	acc_rss_deactivate();

	return EXIT_SUCCESS;
}


bool execute_obstacle_with_blocking_calls(acc_detector_obstacle_configuration_t configuration)
{
	acc_obstacle_t          obstacles[16];
	acc_detector_obstacle_t obstacle_data;
	bool                    result = true;

	obstacle_data.obstacles = obstacles;

	acc_detector_obstacle_handle_t handle = acc_detector_obstacle_create(configuration);
	if (handle == NULL)
	{
		fprintf(stderr, "Failed to create detector.\n");
		return false;
	}

	if (!acc_detector_obstacle_activate(handle))
	{
		fprintf(stderr, "Failed to activate detector.\n");
		acc_detector_obstacle_destroy(&handle);
		return false;
	}

	if (background_estimation_data == NULL)
	{
		acc_detector_obstacle_result_info_t result_info;
		bool completed;

		do
		{
			if (!acc_detector_obstacle_estimate_background(handle, &completed, &result_info))
			{
				fprintf(stderr, "Failed to estimate background.\n");
				acc_detector_obstacle_deactivate(handle);
				acc_detector_obstacle_destroy(&handle);
				return false;
			}
		} while (!completed);

		size_t background_estimation_size = acc_detector_obstacle_background_estimation_get_size(handle);

		background_estimation_data = hal.os.mem_alloc(background_estimation_size);
		if (background_estimation_data != NULL)
		{
			if (!acc_detector_obstacle_background_estimation_get(handle, background_estimation_data))
			{
				acc_detector_obstacle_deactivate(handle);
				acc_detector_obstacle_destroy(&handle);
				return false;
			}
		}
	}
	else
	{
		if (!acc_detector_obstacle_background_estimation_set(handle, background_estimation_data))
		{
			acc_detector_obstacle_deactivate(handle);
			acc_detector_obstacle_destroy(&handle);
			return false;
		}
	}

	for (uint16_t i = 0; i < 20; i++)
	{
		acc_detector_obstacle_result_info_t result_info;

		do
		{
			result = acc_detector_obstacle_get_next(handle, &obstacle_data, &result_info);
		} while (result && !result_info.data_available);

		if (!result)
		{
			break;
		}

		uint8_t nbr_of_obstacles = obstacle_data.nbr_of_obstacles;
		printf("Detected %" PRIu8 " obstacles!\n", nbr_of_obstacles);

		if (nbr_of_obstacles > 0)
		{
			for (uint16_t j = 0; j < nbr_of_obstacles; j++)
			{
				printf("Obstacle at a distance %d mm with amplitude %d\n",
				       (int)(obstacle_data.obstacles[j].distance * 1000.0f),
				       (int)(obstacle_data.obstacles[j].amplitude * 1000.0f));
			}
		}

		printf("\n");
	}

	if (!acc_detector_obstacle_deactivate(handle))
	{
		fprintf(stderr, "Failed to deactivate detector.\n");
		return false;
	}

	acc_detector_obstacle_destroy(&handle);

	return result;
}
