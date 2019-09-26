// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#ifndef ACC_SERVICE_IQ_H_
#define ACC_SERVICE_IQ_H_

#include <complex.h>
#include <stdint.h>

#include "acc_service.h"
#include "acc_sweep_configuration.h"

/**
 * @defgroup IQ IQ Service
 * @ingroup Services
 *
 * @brief IQ Service API description
 *
 * @{
 */


/**
 * @brief Sampling mode
 *
 * The sampling mode corresponds to how point averages are taken. With mode A, averaging
 * is done directly in hardware by repeatedly sampling the same point. With mode B, the
 * averaging is done manually with a special measurement loop. By doing so, a higher SNR
 * is obtained in some situations.
 */
typedef enum
{
	ACC_SERVICE_IQ_SAMPLING_MODE_A,
	ACC_SERVICE_IQ_SAMPLING_MODE_B,
	ACC_SERVICE_IQ_SAMPLING_MODE_COUNT
} acc_service_iq_sampling_mode_enum_t;
typedef uint32_t acc_service_iq_sampling_mode_t;


/**
 * @brief Metadata for the iq service
 */
typedef struct
{
	float    actual_start_m;
	float    actual_length_m;
	uint16_t data_length;
	uint16_t stitch_count;     // Sweep has got stitch_count number of stitches
} acc_service_iq_metadata_t;


/**
 * @brief Metadata for each result provided by the iq service
 */
typedef struct
{
	uint32_t sequence_number;            // Sequence number for the current data
	bool     sensor_communication_error; // Indication of a sensor communication error, service probably needs to be restarted
	bool     data_saturated;             // Indication of sensor data being saturated, can cause result instability
} acc_service_iq_result_info_t;


/**
 * @brief Definition of an iq callback function providing iq data as float (complex)
 */
typedef void (acc_iq_float_callback_t)(const acc_service_handle_t service_handle, const float complex *data,
                                       const acc_service_iq_result_info_t *result_info, void *user_reference);


/**
 * @brief Definition of an iq callback function providing iq data as with real and imaginary components of int16 type
 */
typedef void (acc_iq_int16_callback_t)(const acc_service_handle_t service_handle, const acc_int16_complex_t *data,
                                       const acc_service_iq_result_info_t *result_info, void *user_reference);


/**
 * @brief Create a configuration for an iq service
 *
 * @return Service configuration, NULL if creation was not possible
 */
acc_service_configuration_t acc_service_iq_configuration_create(void);


/**
 * @brief Destroy an iq configuration
 *
 * Destroy an iq configuration that is no longer needed, may be done even if a
 * service has been created with the specific configuration and has not yet been destroyed.
 * The service configuration reference is set to NULL after destruction.
 *
 * @param[in] service_configuration The configuration to destroy, set to NULL
 */
void acc_service_iq_configuration_destroy(acc_service_configuration_t *service_configuration);


/**
 * @brief Get the cutoff for the lowpass filter in the distance domain
 *
 * The cutoff for the distance domain lowpass filter is specified as the ratio between the spatial
 * frequency cutoff and the sample frequency. A ratio close to zero damps everything except the lowest
 * frequencies. Increasing ratios output a wider band of spatial frequencies, and a ratio of 0.5 means
 * that the filter is deactivated.
 *
 * @param[in] service_configuration The configuration to set the distance lowpass filter cutoff for
 * @return The ratio between the configured cutoff frequency and the sampling frequency
 */
float acc_service_iq_depth_lowpass_cutoff_ratio_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set the cutoff for the lowpass filter in the distance domain
 *
 * The cutoff for the distance domain lowpass filter is specified as the ratio between the spatial
 * frequency cutoff and the sample frequency. An input value of zero for the cutoff ratio will
 * configure the smoothest allowed filter. A cutoff ratio of 0.5 turns the filter off.
 *
 * The set of available cutoff frequencies is limited due to internal properties of the filter
 * implementation. Call @ref acc_service_iq_running_average_factor_get to obtain the granted cutoff
 * frequency ratio.
 *
 * @param[in] service_configuration The configuration to set the distance lowpass filter cutoff for
 * @param[in] cutoff_ratio The ratio between the desired cutoff frequency and the sampling frequency
 */
void acc_service_iq_depth_lowpass_cutoff_ratio_set(acc_service_configuration_t service_configuration,
                                                      float cutoff_ratio);


/**
 * @brief Get running average factor
 *
 * The running average factor is the factor of which the most recent sweep is weighed against previous sweeps.
 * Valid range is between 0.0 and 1.0 where 0.0 means that no history is weighed in, i.e filtering is effectively disabled.
 * A factor of 1.0 means that the most recent sweep has no effect on the result,
 * which will result in that the first sweep is forever received as the result.
 *
 * @param[in] service_configuration The configuration to set the running average factor for
 * @return Running average factor
 */
float acc_service_iq_running_average_factor_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set running average factor
 *
 * The running average factor is the factor of which the most recent sweep is weighed against previous sweeps.
 * Valid range is between 0.0 and 1.0 where 0.0 means that no history is weighed in, i.e filtering is effectively disabled.
 * A factor of 1.0 means that the most recent sweep has no effect on the result,
 * which will result in that the first sweep is forever received as the result.
 *
 * @param[in] service_configuration The configuration to set the running average factor for
 * @param[in] factor The running average factor to set
 */
void acc_service_iq_running_average_factor_set(acc_service_configuration_t service_configuration, float factor);


/**
 * @brief Get sampling mode
 *
 * @param[in] service_configuration The configuration to get the sampling mode for
 * @return sampling mode
 */
acc_service_iq_sampling_mode_t acc_service_iq_sampling_mode_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set sampling mode
 *
 * @param[in] service_configuration The configuration to set the sampling mode for
 * @param[in] sampling_mode The sampling mode to use
 */
void acc_service_iq_sampling_mode_set(acc_service_configuration_t service_configuration, acc_service_iq_sampling_mode_t sampling_mode);


/**
 * @brief Set a callback to receive iq results in iq float complex format
 *
 * If a callback is used, iq results are indicated by calling the function that is set.
 * Within the callback it is only allowed to copy the data to a application memory to allow the best
 * possible service execution. Setting the callback to NULL sets the service in blocking mode and results
 * have to be retrieved through calling the acc_service_iq_get_next function.
 *
 * @param[in,out] service_configuration The configuration to set a callback for
 * @param[in] callback The callback function to set
 * @param[in] user_reference A client chosen reference that will be provided when calling the callback
 */
void acc_service_iq_iq_float_callback_set(acc_service_configuration_t service_configuration, acc_iq_float_callback_t *callback,
                                          void *user_reference);


/**
 * @brief Set a callback to receive iq results in iq int16 complex format
 *
 * If a callback is used, iq results are indicated by calling the function that is set.
 * Within the callback it is only allowed to copy the data to a application memory to allow the best
 * possible service execution. Setting the callback to NULL sets the service in blocking mode and results
 * have to be retrieved through calling the acc_service_iq_int16_get_next function.
 *
 * @param[in,out] service_configuration The configuration to set a callback for
 * @param[in] callback The callback function to set
 * @param[in] user_reference A client chosen reference that will be provided when calling the callback
 */
void acc_service_iq_iq_int16_callback_set(acc_service_configuration_t service_configuration, acc_iq_int16_callback_t *callback,
                                          void *user_reference);


/**
 * @brief Configure the service in blocking mode with float complex output
 *
 * Call @ref acc_service_iq_get_next to retrieve the output data from this mode.
 *
 * @param[in,out] service_configuration The configuration to set a callback for
 */
void acc_service_iq_blocking_float_output_set(acc_service_configuration_t service_configuration);


/**
 * @brief Configure the service in blocking mode with int16 complex output
 *
 * Call @ref acc_service_iq_int16_get_next to retrieve the output data from this mode.
 *
 * @param[in,out] service_configuration The configuration to set a callback for
 */
void acc_service_iq_blocking_int16_output_set(acc_service_configuration_t service_configuration);


/**
 * @brief Get service metadata
 *
 * May only be called after a service has been created.
 *
 * @param[in] handle The service handle for the service to get metadata for
 * @param[out] metadata Metadata results are provided in this parameter
 */
void acc_service_iq_get_metadata(acc_service_handle_t handle, acc_service_iq_metadata_t *metadata);


/**
 * @brief Retrieve the next result from the service
 *
 * May only be called after a service has been activated to retrieve the next result, blocks
 * the application until a result is ready. The service must be configured for floating point output.
 * It is not possible to use this blocking call and callbacks simultaneously.
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data IQ data result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info IQ result info, sending in NULL is ok
 * @return Service status
 */
acc_service_status_t acc_service_iq_get_next(acc_service_handle_t handle, float complex *data, uint16_t data_length,
                                             acc_service_iq_result_info_t *result_info);


/**
 * @brief Retrieve the next result from the service
 *
 * May only be called after a service has been activated to retrieve the next result, blocks
 * the application until a result is ready. The service must be configured for int16 complex output.
 * It is not possible to use this blocking call and callbacks simultaneously.
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data IQ data result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info IQ result info, sending in NULL is ok
 * @return Service status
 */
acc_service_status_t acc_service_iq_int16_get_next(acc_service_handle_t handle, acc_int16_complex_t *data, uint16_t data_length,
                                                   acc_service_iq_result_info_t *result_info);


/**
 * @brief Execute service one time
 *
 * Activates service, produces one result and then deactivates the service. Blocks the
 * application until a service result has been produced. Will not work if a callback
 * has been set and may fail if the service is already active. The service must be configured
 * for floating point output.
 *
 * @param[in] handle The service handle for the service to execute
 * @param[out] data IQ data result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info IQ result info, sending in NULL is ok
 * @return Service status
 */
acc_service_status_t acc_service_iq_execute_once(acc_service_handle_t handle, float complex *data, uint16_t data_length,
                                                 acc_service_iq_result_info_t *result_info);


/**
 * @brief Execute service one time
 *
 * Activates service, produces one result and then deactivates the service. Blocks the
 * application until a service result has been produced. Will not work if a callback
 * has been set and may fail if the service is already active. The service must be configured
 * for int16 complex output.
 *
 * @param[in] handle The service handle for the service to execute
 * @param[out] data IQ data result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info IQ result info, sending in NULL is ok
 * @return Service status
 */
acc_service_status_t acc_service_iq_int16_execute_once(acc_service_handle_t handle, acc_int16_complex_t *data, uint16_t data_length,
                                                       acc_service_iq_result_info_t *result_info);


/**
 * @}
 */

#endif
