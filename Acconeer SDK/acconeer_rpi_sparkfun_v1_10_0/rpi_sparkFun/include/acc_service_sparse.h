// Copyright (c) Acconeer AB, 2019
// All rights reserved

#ifndef ACC_SERVICE_SPARSE_H_
#define ACC_SERVICE_SPARSE_H_

#include <stdint.h>

#include "acc_service.h"
#include "acc_sweep_configuration.h"

/**
 * @defgroup Sparse Sparse Service
 * @ingroup Services
 *
 * @brief Sparse service API description
 *
 * @{
 */


/**
 * @brief Sampling mode
 *
 * The sampling mode changes how the hardware accelerated averaging is done. Mode A is optimized
 * for maximal independence of the depth points, giving a higher depth resolution than mode B.
 * Mode B is instead optimized for maximal SNR per unit time spent on measuring. This makes it
 * more energy efficient and suitable for cases where small movements are to be detected over long
 * ranges. Mode A is more suitable for applications like gesture recognition, measuring the
 * distance to a movement, and speed measurements.
 *
 * Mode B typically gives roughly 3 dB better SNR per unit time than mode A. However, please note
 * that very short ranges of only one or a few points are suboptimal with mode B. In those cases,
 * always use mode A.
 */
typedef enum
{
	ACC_SERVICE_SPARSE_SAMPLING_MODE_A,
	ACC_SERVICE_SPARSE_SAMPLING_MODE_B
} acc_service_sparse_sampling_mode_enum_t;
typedef uint32_t acc_service_sparse_sampling_mode_t;


/**
 * @brief Metadata for the sparse service
 */
typedef struct
{
	float    actual_start_m;
	float    actual_length_m;
	uint16_t data_length;
	float    actual_sweep_rate;
	float    actual_stepsize_m;
} acc_service_sparse_metadata_t;


/**
 * @brief Metadata for each result provided by the sparse service
 */
typedef struct
{
	bool     sensor_communication_error; // Indication of a sensor communication error, service probably needs to be restarted
	bool     data_saturated;             // Indication of sensor data being saturated, can cause result instability
	uint32_t sequence_number;            // Sequence number for the current data
} acc_service_sparse_result_info_t;


/**
 * @brief Definition of a sparse callback function
 */
typedef void (acc_sparse_callback_t)(const acc_service_handle_t service_handle, const uint16_t *data,
                                     const acc_service_sparse_result_info_t *result_info, void *user_reference);


/**
 * @brief Create a configuration for a sparse service
 *
 * @return Service configuration, NULL if creation was not possible
 */
acc_service_configuration_t acc_service_sparse_configuration_create(void);


/**
 * @brief Destroy a sparse configuration
 *
 * Destroy a sparse configuration that is no longer needed, may be done even if a
 * service has been created with the specific configuration and has not yet been destroyed.
 * The service configuration reference is set to NULL after destruction.
 *
 * @param[in] service_configuration The configuration to destroy, set to NULL
 */
void acc_service_sparse_configuration_destroy(acc_service_configuration_t *service_configuration);


/**
 * @brief Get the number of sweeps per service frame
 *
 * Gets the number of sweeps that will be returned in each frame from the service.
 *
 * @param[in] service_configuration The service configuration to get sweeps per result from
 * @return sweeps per frame
 */
uint16_t acc_service_sparse_configuration_sweeps_per_frame_get(const acc_service_configuration_t service_configuration);


/**
 * @brief Set sweeps per service frame
 *
 * Sets the number of sweeps that will be returned in each frame from the service.
 *
 * For sampling mode B, the number of sweeps per frame must be less than or equal to 64.
 *
 * @param[in] service_configuration The service configuration to set sweeps per results in
 * @param[in] sweeps Sweeps per frame
 */
void acc_service_sparse_configuration_sweeps_per_frame_set(acc_service_configuration_t service_configuration, uint16_t sweeps);


/**
 * @brief Get the sweep rate
 *
 * Gets the requested sweep rate. Values of zero of lower are treated as the maximum possible rate.
 *
 * @param[in] service_configuration The service configuration to get the sweep rate from
 * @return sweep_rate The sweep rate
 */
float acc_service_sparse_configuration_sweep_rate_get(const acc_service_configuration_t service_configuration);


/**
 * @brief Set sweep rate
 *
 * Sets the requested sweep rate. Values of zero of lower are treated as the maximum possible rate.
 *
 * @param[in] service_configuration The service configuration to set the sweep rate in
 * @param[in] sweep_rate The sweep rate
 */
void acc_service_sparse_configuration_sweep_rate_set(acc_service_configuration_t service_configuration, float sweep_rate);


/**
 * @brief Get sampling mode
 *
 * @param[in] service_configuration The configuration to get the sampling mode for
 * @return sampling mode
 */
acc_service_sparse_sampling_mode_t acc_service_sparse_sampling_mode_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set sampling mode
 *
 * @param[in] service_configuration The configuration to set the sampling mode for
 * @param[in] sampling_mode The sampling mode to use
 */
void acc_service_sparse_sampling_mode_set(acc_service_configuration_t service_configuration, acc_service_sparse_sampling_mode_t sampling_mode);


/**
 * @brief Get the sensor stepsize
 *
 * Gets the stepsize - the number of steps taken between each data point. A stepsize of 1 samples
 * every possible point in the range. A stepsize of 2 samples every other point, and so on.
 *
 * In the sparse service, the base step length is ~6cm. Thus, for example setting stepsize to 3 makes
 * the distance between two points in the measured range ~18cm.
 *
 * @param[in] service_configuration The configuration to get stepsize from
 * @return stepsize The stepsize
 */
uint16_t acc_service_sparse_stepsize_get(acc_service_configuration_t service_configuration);


/**
 * @brief Set the sensor stepsize
 *
 * Sets the stepsize - the number of steps taken between each data point. A stepsize of 1 samples
 * every possible point in the range. A stepsize of 2 samples every other point, and so on.
 *
 * In the sparse service, the base step length is ~6cm. Thus, for example setting stepsize to 3 makes
 * the distance between two points in the measured range ~18cm.
 *
 * The sparse service supports setting an arbitrary stepsize of at least 1.
 *
 * @param[in] service_configuration The configuration to set stepsize in
 * @param[in] stepsize The stepsize
 */
void acc_service_sparse_stepsize_set(acc_service_configuration_t service_configuration, uint16_t stepsize);


/**
 * @brief Set a callback to receive sparse results
 *
 * If a callback is used, sparse results are indicated by calling the function that is set.
 * Within the callback it is only allowed to copy the data to a application memory to allow the best
 * possible service execution. Setting the callback as NULL disables callback operation.
 *
 * @param[in] service_configuration The configuration to set a callback for
 * @param[in] callback The callback function to set
 * @param[in] user_reference A user chosen reference that will be provided when calling the callback
 */
void acc_service_sparse_sparse_callback_set(acc_service_configuration_t service_configuration, acc_sparse_callback_t *callback, void *user_reference);


/**
 * @brief Get service metadata
 *
 * May only be called after a service has been created.
 *
 * @param[in] handle The service handle for the service to get metadata for
 * @param[out] metadata Metadata results are provided in this parameter
 */
void acc_service_sparse_get_metadata(acc_service_handle_t handle, acc_service_sparse_metadata_t *metadata);


/**
 * @brief Retrieve the next result from the service
 *
 * May only be called after a service has been activated to retrieve the next result, blocks
 * the application until a result is ready.
 * It is not possible to use this blocking call and callbacks simultaneously.
 *
 * @param[in] handle The service handle for the service to get the next result for
 * @param[out] data sparse result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info Sparse result info, sending in NULL is ok
 * @return Service status
 */
acc_service_status_t acc_service_sparse_get_next(acc_service_handle_t handle, uint16_t *data, uint16_t data_length,
                                                 acc_service_sparse_result_info_t *result_info);


/**
 * @brief Execute service one time
 *
 * Activates service, produces one result and then deactivates the service. Blocks the
 * application until a service result has been produced. Will not work if a callback
 * has been set and may fail if the service is already active.
 *
 * @param[in] handle The service handle for the service to execute
 * @param[out] data sparse result
 * @param[in] data_length The length of the buffer provided for the result
 * @param[out] result_info Sparse result info, sending in NULL is ok
 * @return Service status
 */
acc_service_status_t acc_service_sparse_execute_once(acc_service_handle_t handle, uint16_t *data, uint16_t data_length,
                                                     acc_service_sparse_result_info_t *result_info);


/**
 * @}
 */

#endif
