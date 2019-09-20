// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#ifndef ACC_SERVICE_ENVELOPE_H_
#define ACC_SERVICE_ENVELOPE_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_service.h"
#include "acc_sweep_configuration.h"

/**
 * @defgroup Envelope Envelope Service
 * @ingroup Services
 *
 * @brief Envelope service API description
 *
 * @{
 */


/**
 * @brief Predefined profiles allowing fast setup of an envelope configuration
 *
 * A profile configures the RX/TX path in the sensor and sets all other configuration
 * parameters to a predefined value
 *
 * DIRECT_LEAKAGE is an experimental profile
 */
typedef enum
{
	ACC_SERVICE_ENVELOPE_PROFILE_MAXIMIZE_DEPTH_RESOLUTION,
	ACC_SERVICE_ENVELOPE_PROFILE_MAXIMIZE_SNR,
	ACC_SERVICE_ENVELOPE_PROFILE_DIRECT_LEAKAGE, // Experimental feature
	ACC_SERVICE_ENVELOPE_PROFILE_MAX,
	ACC_SERVICE_ENVELOPE_PROFILE_DEFAULT = ACC_SERVICE_ENVELOPE_PROFILE_MAXIMIZE_DEPTH_RESOLUTION
} acc_service_envelope_profile_enum_t;
typedef uint32_t acc_service_envelope_profile_t;


/**
 * @brief Metadata for the envelope service
 */
typedef struct
{
	float    actual_start_m;
	float    actual_length_m;
	uint16_t data_length;
	uint16_t stitch_count;     // Sweep has got stitch_count number of stitches
} acc_service_envelope_metadata_t;


/**
 * @brief Metadata for each result provided by the envelope service
 */
typedef struct
{
	uint32_t sequence_number;            // Sequence number for the current data
	bool     sensor_communication_error; // Indication of a sensor communication error, service probably needs to be restarted
	bool     data_saturated;             // Indication of sensor data being saturated, can cause result instability
} acc_service_envelope_result_info_t;


/**
 * @brief Definition of an envelope callback function
 */
typedef void (acc_envelope_callback_t)(const acc_service_handle_t service_handle, const uint16_t *data,
                                       const acc_service_envelope_result_info_t *result_info, void *user_reference);


/**
 * @brief Create a configuration for an envelope service
 *
 * @return Service configuration, NULL if creation was not possible
 */
acc_service_configuration_t acc_service_envelope_configuration_create(void);


/**
 * @brief Destroy an envelope configuration
 *
 * Destroy an envelope configuration that is no longer needed, may be done even if a
 * service has been created with the specific configuration and has not yet been destroyed.
 * The service configuration reference is set to NULL after destruction.
 *
 * @param[in] service_configuration The configuration to destroy, set to NULL
 */
void acc_service_envelope_configuration_destroy(acc_service_configuration_t *service_configuration);


/**
 * @brief Get a profile of configuration parameters
 *
 * A profile consists of a number of settings for the sensor as well as the sweep setup to
 * allow a quick and easy setup of a service.
 * Most parameters may be specifically set using their own set methods while the sensor
 * specific settings of e.g. transmitted energy and receiver gain are matched to the profile
 * used.
 *
 * @param[in] service_configuration The configuration to get a profile for
 * @return The current profile, ACC_SERVICE_ENVELOPE_PROFILE_MAX if configuration is invalid
 */
acc_service_envelope_profile_t acc_service_envelope_profile_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set a profile of configuration parameters
 *
 * A profile consists of a number of settings for the sensor as well as the sweep setup to
 * allow a quick and easy setup of a service.
 * Most parameters may be specifically set using their own set methods while the sensor
 * specific settings of e.g. transmitted energy and receiver gain are matched to the profile
 * used.
 *
 * @param[in] service_configuration The configuration to set a profile for
 * @param[in] profile The profile to set
 */
void acc_service_envelope_profile_set(acc_service_configuration_t service_configuration, acc_service_envelope_profile_t profile);


/**
 * @brief Get compensate phase flag
 *
 * The compensate phase flag indicates whether a phase compensation is done during filtering of data.
 * Using phase compensation results in more stable data but can suppress detection of fast moving reflections.
 * The default is on.
 *
 * @param[in] service_configuration The configuration to get the compensate phase flag for
 * @return The current value of the flag
 */
bool acc_service_envelope_compensate_phase_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set compensate phase flag
 *
 * The compensate phase flag indicates whether a phase compensation is done during filtering of data.
 * Using phase compensation results in more stable data but can suppress detection of fast moving reflections.
 * The default is on.
 *
 * @param[in] service_configuration The configuration to set the compensate phase flag for
 * @param[in] compensate_phase The compensate phase flag to set, true to use phase compensation, false otherwise
 */
void acc_service_envelope_compensate_phase_set(acc_service_configuration_t service_configuration, bool compensate_phase);


/**
 * @brief Get running average factor
 *
 * The running average factor is the factor of which the most recent sweep is weighed against previous sweeps.
 * Valid range is between 0.0 and 1.0 where 0.0 means that no history is weighed in, i.e filtering is effectively disabled.
 * A factor of 1.0 means that the most recent sweep has no effect on the result,
 * which will result in that the first sweep is forever received as the result.
 * The filtering is coherent and is done on complex valued IQ data before conversion to envelope data.
 *
 * @param[in] service_configuration The configuration to get the running average factor for
 * @return Running average factor
 */
float acc_service_envelope_running_average_factor_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set running average factor
 *
 * The running average factor is the factor of which the most recent sweep is weighed against previous sweeps.
 * Valid range is between 0.0 and 1.0 where 0.0 means that no history is weighed in, i.e filtering is effectively disabled.
 * A factor of 1.0 means that the most recent sweep has no effect on the result,
 * which will result in that the first sweep is forever received as the result.
 * The filtering is coherent and is done on complex valued IQ data before conversion to envelope data.
 *
 * @param[in] service_configuration The configuration to set the running average factor for
 * @param[in] factor The running average factor to set
 */
void acc_service_envelope_running_average_factor_set(acc_service_configuration_t service_configuration, float factor);


/**
 * @brief Set a callback to receive envelope results
 *
 * If a callback is used, envelope results are indicated by calling the function that is set.
 * Within the callback it is only allowed to copy the data to a application memory to allow the best
 * possible service execution. Setting the callback as NULL disables callback operation.
 *
 * @param[in] service_configuration The configuration to set a callback for
 * @param[in] envelope_callback The callback function to set
 * @param[in] user_reference A user chosen reference that will be provided when calling the callback
 */
void acc_service_envelope_envelope_callback_set(acc_service_configuration_t service_configuration, acc_envelope_callback_t *envelope_callback,
                                                       void *user_reference);


/**
 * @brief Get service metadata
 *
 * May only be called after a service has been created.
 *
 * @param[in] handle The service handle for the service to get metadata for
 * @param[out] metadata Metadata results are provided in this parameter
 */
void acc_service_envelope_get_metadata(acc_service_handle_t handle, acc_service_envelope_metadata_t *metadata);


/**
 * @brief Retrieve the next result from the service
 *
 * May only be called after a service has been activated to retrieve the next result, blocks
 * the application until a result is ready.
 * It is not possible to use this blocking call and callbacks simultaneously.
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data Envelope result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info Envelope result info, sending in NULL is ok
 * @return Service status
 */
acc_service_status_t acc_service_envelope_get_next(acc_service_handle_t handle, uint16_t *data, uint16_t data_length,
                                                   acc_service_envelope_result_info_t *result_info);


/**
 * @brief Execute service one time
 *
 * Activates service, produces one result and then deactivates the service. Blocks the
 * application until a service result has been produced. Will not work if a callback
 * has been set and may fail if the service is already active.
 *
 * @param[in] handle The service handle for the service to execute
 * @param[out] data Envelope result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info Envelope result info, sending in NULL is ok
 * @return Service status
 */
acc_service_status_t acc_service_envelope_execute_once(acc_service_handle_t handle, uint16_t *data, uint16_t data_length,
                                                       acc_service_envelope_result_info_t *result_info);


/**
 * @}
 */

#endif
