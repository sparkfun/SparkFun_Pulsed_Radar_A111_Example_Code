// Copyright (c) Acconeer AB, 2019
// All rights reserved

#ifndef ACC_RSS_DIAGNOSTICS_H_
#define ACC_RSS_DIAGNOSTICS_H_

#include "acc_definitions.h"
#include <stdbool.h>


/**
 * @defgroup Diagnostic_test Diagnostic test
 * @ingroup RSS
 *
 * @brief RSS Diagnostic test
 *
 * @{
 */


/**
 * @brief Run diagnostic test
 *
 * This function executes a suite of tests suitable for customer diagnostic testing.
 *
 * @details If a test fails to execute the return value will be set to false.
 *
 * @param[in] sensor_id The sensor to run diagnostic test on
 * @return True if successfully run, false otherwise
 */
bool acc_rss_diagnostic_test(acc_sensor_id_t sensor_id);


/**
 * @}
 */

#endif
