// Copyright (c) Acconeer AB, 2019
// All rights reserved

#ifndef ACC_DETECTOR_PRESENCE_H_
#define ACC_DETECTOR_PRESENCE_H_

#include <stdbool.h>
#include <stdint.h>

#include "acc_sweep_configuration.h"


/**
 * @brief Presence detector handle
 */
struct acc_detector_presence_handle;

typedef struct acc_detector_presence_handle *acc_detector_presence_handle_t;


/**
 * @brief Presence detector configuration container
 */
struct acc_detector_presence_configuration;

typedef struct acc_detector_presence_configuration *acc_detector_presence_configuration_t;


/**
 * @brief Parameter structure for data filtering settings
 */
typedef struct
{
	/** Time constant in s for inter-frame signal smoothing after the bandpass filtering */
	float inter_frame_deviation_time_const;
	/** Low pass cutoff frequency in Hz for inter-frame signal bandpass filtering */
	float inter_frame_fast_cutoff;
	/** High pass cutoff frequency in Hz for inter-frame signal bandpass filtering */
	float inter_frame_slow_cutoff;
	/** Time constant in s for inter-frame signal smoothing */
	float intra_frame_time_const;
	/** Weight between 0 and 1 for presence score interpolation between the inter-frame and intra-frame signals */
	float intra_frame_weight;
	/** Time constant in s for smoothing of the presence score */
	float output_time_const;
} acc_detector_presence_configuration_filter_parameters_t;


/**
 * @brief Presence detector results container
 */
typedef struct
{
	bool  presence_detected;
	float presence_score;
	float presence_distance;
} acc_detector_presence_result_t;


/**
 * @brief Create a configuration for a presence detector
 *
 * @return Presence detector configuration, NULL if creation was not possible
 */
acc_detector_presence_configuration_t acc_detector_presence_configuration_create(void);


/**
 * @brief Destroy a presence detector configuration
 *
 * @param[in] presence_configuration The configuration to destroy, set to NULL
 */
void acc_detector_presence_configuration_destroy(acc_detector_presence_configuration_t *presence_configuration);


/**
 * @brief Create a presence detector with the provided configuration
 *
 * Only one presence detector may exist for a specific sensor at any given time and
 * invalid configurations will not allow for presence detector creation.
 *
 * @param[in] presence_configuration The presence detector configuration to create a presence detector with
 * @return Presence detector handle, NULL if presence detector was not possible to create
 */
acc_detector_presence_handle_t acc_detector_presence_create(const acc_detector_presence_configuration_t presence_configuration);


/**
 * @brief Destroy a presence detector identified with the provided handle
 *
 * Destroy the context of a presence detector allowing another presence detector to be created using the
 * same resources. The presence detector handle reference is set to NULL after destruction.
 * If NULL is sent in, nothing happens.
 *
 * @param[in] presence_handle A reference to the presence detector handle to destroy
 */
void acc_detector_presence_destroy(acc_detector_presence_handle_t *presence_handle);


/**
 * @brief Reconfigure a presence detector with the provided configuration
 *
 * Only one presence detector may exist for a specific sensor at any given time and
 * invalid reconfigurations will not allow for presence detector creation.
 *
 * @param[in] presence_handle A reference to the presence detector handle to reconfigure
 * @param[in] presence_configuration The presence detector configuration to reconfigure a presence detector with
 * @return True if possible to reconfigure
 */
bool acc_detector_presence_reconfigure(acc_detector_presence_handle_t              *presence_handle,
                                       const acc_detector_presence_configuration_t presence_configuration);


/**
 * @brief Activate the presence detector associated with the provided handle
 *
 * @param[in] presence_handle The presence detector handle for the presence detector to activate
 * @return True if successful, otherwise false
 */
bool acc_detector_presence_activate(acc_detector_presence_handle_t presence_handle);


/**
 * @brief Deactivate the presence detector associated with the provided handle
 *
 * @param[in] presence_handle The presence detector handle for the presence detector to deactivate
 * @return True if successful, otherwise false
 */
bool acc_detector_presence_deactivate(acc_detector_presence_handle_t presence_handle);


/**
 * @brief Retrieve the next result from the presence detector
 *
 * May only be called after a presence detector has been activated, blocks
 * the application until a result is ready.
 *
 * @param[in] presence_handle The presence detector handle for the presence detector to get the next result for
 * @param[out] result Presence detector results
 * @return True if successful, otherwise false
 */
bool acc_detector_presence_get_next(acc_detector_presence_handle_t presence_handle, acc_detector_presence_result_t *result);


/**
 * @brief Get sweep range start
 *
 * @param[in] configuration The configuration to get the sweep range start for
 * @return requested sweep range start in m
 */
float acc_detector_presence_configuration_range_start_get(const acc_detector_presence_configuration_t configuration);


/**
 * @brief Set sweep range start
 *
 * @param[in] configuration The configuration to set the sweep range start for
 * @param[in] range_start The requested sweep range start in m
 */
void acc_detector_presence_configuration_range_start_set(acc_detector_presence_configuration_t configuration, float range_start);


/**
 * @brief Get sweep range length
 *
 * @param[in] configuration The configuration to get the sweep range length for
 * @return requested sweep range length in m
 */
float acc_detector_presence_configuration_range_length_get(const acc_detector_presence_configuration_t configuration);


/**
 * @brief Set sweep range length
 *
 * @param[in] configuration The configuration to set the requested sweep range length for
 * @param[in] range_length The requested sweep range length in m
 */
void acc_detector_presence_configuration_range_length_set(acc_detector_presence_configuration_t configuration, float range_length);


/**
 * @brief Get sensor ID
 *
 * @param[in] configuration The configuration to get the sensor ID for
 * @return sensor ID
 */
acc_sensor_id_t acc_detector_presence_configuration_sensor_get(const acc_detector_presence_configuration_t configuration);


/**
 * @brief Set sensor ID
 *
 * @param[in] configuration The configuration to set the sensor ID for
 * @param[in] sensor The sensor ID
 */
void acc_detector_presence_configuration_sensor_set(acc_detector_presence_configuration_t configuration, acc_sensor_id_t sensor);


/**
 * @brief Get detection threshold
 *
 * @param[in] configuration The configuration to get the detection threshold for
 * @return detection threshold
 */
float acc_detector_presence_configuration_detection_threshold_get(const acc_detector_presence_configuration_t configuration);


/**
 * @brief Set detection threshold
 *
 * @param[in] configuration The configuration to set the detection threshold for
 * @param[in] detection_threshold The threshold
 */
void acc_detector_presence_configuration_detection_threshold_set(acc_detector_presence_configuration_t configuration,
                                                                 float                                 detection_threshold);


/**
 * @brief Get detection update rate
 *
 * @param[in] configuration The configuration to get the detection update rate for
 * @return detection update rate
 */
float acc_detector_presence_configuration_update_rate_get(const acc_detector_presence_configuration_t configuration);


/**
 * @brief Set detection update rate
 *
 * @param[in] configuration The configuration to set the detection update rate for
 * @param[in] update_rate
 */
void acc_detector_presence_configuration_update_rate_set(acc_detector_presence_configuration_t configuration,
                                                         float                                 update_rate);


/**
 * @brief Get the number of sweeps per frame
 *
 * @param[in] configuration The configuration to get the number of sweeps per frame for
 * @return The number of sweeps per frame
 */
uint16_t acc_detector_presence_configuration_sweeps_per_frame_get(const acc_detector_presence_configuration_t configuration);


/**
 * @brief Set the number of sweeps per frame
 *
 * @param[in] configuration The configuration to set the number of sweeps per frame for
 * @param[in] sweeps_per_frame The requested number of sweeps per frame
 */
void acc_detector_presence_configuration_sweeps_per_frame_set(acc_detector_presence_configuration_t configuration,
                                                              uint16_t                              sweeps_per_frame);


/**
 * @brief Get sensor data filtering parameters
 *
 * See @ref acc_detector_presence_configuration_filter_parameters_set
 *
 * @param[in] configuration The configuration to get the filter parameters for
 * @return The filter parameters, see @ref acc_detector_presence_configuration_filter_parameters_t
 */
acc_detector_presence_configuration_filter_parameters_t acc_detector_presence_configuration_filter_parameters_get(
	const acc_detector_presence_configuration_t configuration);


/**
 * @brief Set sensor data filtering parameters
 *
 * Set filter parameters for the sensor data filtering that is performed before presence detection thresholding.
 *
 * @param[in] configuration The configuration to set the filter parameters for
 * @param[in] filter_parameters The filter parameter structure, see @ref acc_detector_presence_configuration_filter_parameters_t
 */
void acc_detector_presence_configuration_filter_parameters_set(acc_detector_presence_configuration_t                         configuration,
                                                               const acc_detector_presence_configuration_filter_parameters_t *filter_parameters);


/**
 * @brief Get power save mode
 *
 * @param[in] configuration The configuration to get the power save mode for
 * @return power save mode
 */
acc_sweep_configuration_power_save_mode_t acc_detector_presence_configuration_power_save_mode_get(
	const acc_detector_presence_configuration_t configuration);


/**
 * @brief Set power save mode
 *
 * @param[in] configuration The configuration to set the power save mode for
 * @param[in] power_save_mode The power save mode
 */
void acc_detector_presence_configuration_power_save_mode_set(acc_detector_presence_configuration_t     configuration,
                                                             acc_sweep_configuration_power_save_mode_t power_save_mode);


#endif
