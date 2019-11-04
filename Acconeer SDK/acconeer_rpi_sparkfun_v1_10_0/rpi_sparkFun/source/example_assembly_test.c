// Copyright (c) Acconeer AB, 2019
// All rights reserved

#include <stdio.h>

#include "acc_driver_hal.h"
#include "acc_rss.h"
#include "acc_rss_assembly_test.h"
#include "acc_version.h"


#define DEFAULT_SENSOR_ID 1


static acc_hal_t hal;


int main(void)
{
	acc_rss_assembly_test_result_t test_results[ACC_RSS_ASSEMBLY_TEST_MAX_NUMBER_OF_TESTS];
	uint16_t                       nr_of_test_results = ACC_RSS_ASSEMBLY_TEST_MAX_NUMBER_OF_TESTS;
	bool                           success;
	int exit_code = 0;

	if (!acc_driver_hal_init())
	{
		return EXIT_FAILURE;
	}

	printf("Acconeer software version %s\n", ACC_VERSION);
	printf("Acconeer RSS version %s\n", acc_rss_version());

	hal = acc_driver_hal_get_implementation();

	if (!acc_rss_activate_with_hal(&hal))
	{
		return EXIT_FAILURE;
	}

	acc_rss_assembly_test_configuration_t configuration = acc_rss_assembly_test_configuration_create();

	acc_rss_assembly_test_configuration_sensor_set(configuration, DEFAULT_SENSOR_ID);

	success = acc_rss_assembly_test(configuration, test_results, &nr_of_test_results);

	acc_rss_assembly_test_configuration_destroy(&configuration);

	if (success)
	{
		bool all_passed = true;

		for (int i = 0; i < nr_of_test_results; i++)
		{
			const bool test_passed = test_results[i].test_passed;
			printf("Name: %s, result: %s\n", test_results[i].test_name, test_passed ? "Pass" : "Fail");
			all_passed = test_passed ? all_passed : false;
		}

		if (all_passed)
		{
			printf("Assembly test: All tests passed\n");
		}
		else
		{
			printf("Assembly test: There are failed tests\n");
			exit_code = 1;
		}
	}
	else
	{
		printf("Assembly test: Failed to complete\n");
		exit_code = 1;
	}

	return exit_code;
}
