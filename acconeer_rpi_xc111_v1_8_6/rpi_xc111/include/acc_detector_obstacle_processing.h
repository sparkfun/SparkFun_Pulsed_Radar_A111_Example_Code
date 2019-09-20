// Copyright (c) Acconeer AB, 2019
// All rights reserved

#ifndef ACC_DETECTOR_OBSTACLE_PROCESSING_H_
#define ACC_DETECTOR_OBSTACLE_PROCESSING_H_

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#include "acc_definitions.h"
#include "acc_detector_obstacle_configuration.h"
#include "acc_service_iq.h"
#include "acc_sweep_configuration.h"

/**
 * @addtogroup Obstacle
 *
 * @brief Obstacle processing API description
 *
 * @{
 */


/**
 * @brief Obstacle processing handle
 */
struct acc_detector_obstacle_processing_handle;

typedef struct acc_detector_obstacle_processing_handle *acc_detector_obstacle_processing_handle_t;


/**
 * @brief A struct representing a detected obstacle
 *
 * @param radial_velocity The radial velocity of the obstacle
 * @param distance The distance in m from the sensor where the obstacle is located
 * @param amplitude The detected amplitude in the reflection from the obstacle
 */
typedef struct
{
	float radial_velocity;
	float distance;
	float amplitude;
} acc_obstacle_t;


/**
 * A struct containing information about detected obstacles
 *
 * @param nbr_of_obstacles The number of obstacles detected at a given time
 * @param obstacles An array containing information about detected obstacles. The array contains obstacles_detected elements.
 */
typedef struct
{
	uint8_t        nbr_of_obstacles;
	acc_obstacle_t *obstacles;
} acc_detector_obstacle_t;


/**
 * @brief Get the size of the buffer used in acc_detector_obstacle_processing_init()
 *
 * @param[in] obstacle_configuration The configuration to use
 * @param[in] iq_metadata The IQ metadata
 *
 * @return The size of the buffer needed for acc_detector_obstacle_processing_init()
 */
size_t acc_detector_obstacle_processing_get_size(const acc_detector_obstacle_configuration_t obstacle_configuration,
                                                 const acc_service_iq_metadata_t             *iq_metadata);


/**
 * @brief Get the size of the background estimation buffer.
 *
 * @param[in] handle Obstacle handle
 * @return Size of the buffer needed
 */
size_t acc_detector_obstacle_processing_background_estimation_get_size(const acc_detector_obstacle_processing_handle_t handle);


/**
 * @brief Init an obstacle detector processing instance.
 *
 * @param[in, out] buffer The buffer used to initialize, must be at least acc_detector_obstacle_processing_get_size()
 *                        bytes long and suitably aligned for any built-in type. This buffer must remain valid
 *                        as long the handle returned is used.
 * @param[in] obstacle_configuration The configuration to use
 * @param[in] iq_metadata The IQ metadata
 * @param[in] iq_lowpass_cutoff_ratio The ratio between the spatial cutoff frequency and the IQ-data sampling density
 *                                    for the depth domain lowpass filter
 *
 * @return A handle for the initialized obstacle processing or a NULL if initialization failed or buffer was NULL
 */
acc_detector_obstacle_processing_handle_t acc_detector_obstacle_processing_init(void                                        *buffer,
                                                                                const acc_detector_obstacle_configuration_t obstacle_configuration,
                                                                                const acc_service_iq_metadata_t             *iq_metadata,
                                                                                float                                       iq_lowpass_cutoff_ratio);


/**
 * @brief Estimate background
 *
 * This function process the iq_data assuming that the data is captured without any obstacles. The
 * data is used to estimate the background and will later be used to increase accuracy of the
 * detection algorithm.
 *
 * This function should be called repeatedly with new IQ data until completed is set to true.
 *
 * @param[in] handle The handle
 * @param[in] iq_data IQ Data to be processed
 * @param[out] completed true when background estimation is finished
 *
 * @return True if successful, otherwise false
 */
bool acc_detector_obstacle_processing_estimate_background(acc_detector_obstacle_processing_handle_t handle,
                                                          acc_int16_complex_t                       *iq_data,
                                                          bool                                      *completed);


/**
 * @brief Get background estimation data
 *
 * @param[in] handle The handle
 * @param[out] background_estimation Buffer used for background estimation data. Must be at least
 *                                   acc_detector_obstacle_processing_background_estimation_get_size() bytes long.
 *
 * @return True if successful, otherwise false
 */
bool acc_detector_obstacle_processing_background_estimation_get(acc_detector_obstacle_processing_handle_t handle,
                                                                uint8_t                                   *background_estimation);


/**
 * @brief Set background estimation data
 *
 * Data will be copied to detector
 *
 * @param[in] handle The handle
 * @param[in] background_estimation The background estimation data to be used.
 *
 * @return True if successful, otherwise false
 */
bool acc_detector_obstacle_processing_background_estimation_set(acc_detector_obstacle_processing_handle_t handle,
                                                                const uint8_t                             *background_estimation);


/**
 * @brief Process sensor data
 *
 * This function process the iq_data and tries to find obstacles. If the processing is completed
 * and the obstacle_data is valid, data_available is set to true, otherwise more iq_data is needed
 * and should be provided in more calls to this functions.
 *
 * @param[in] handle The handle
 * @param[in] iq_data IQ Data to be processed
 * @param[out] obstacle_data Information about detected obstacles that is going to be sent to the user
 * @param[out] data_available true if new obstacle data was available in obstacle_data
 *
 * @return True if successful, otherwise false
 */
bool acc_detector_obstacle_processing_next(acc_detector_obstacle_processing_handle_t handle,
                                           acc_int16_complex_t                       *iq_data,
                                           acc_detector_obstacle_t                   *obstacle_data,
                                           bool                                      *data_available);


/**
 * @brief Convert radial velocity to angle in degrees
 *
 * The radial velocity will be converted to the actual angle in degrees.
 *
 * @param[in] speed The current speed
 * @param[in] radial_velocity Radial velocity of obstacle
 *
 *               obstacle
 *              _____o
 *              |   /
 *        speed |  / radial_velocity
 *              | /
 *              |/
 *              o
 *           sensor
 *
 * @return Angle in degrees
 */
float acc_detector_obstacle_processing_radial_velocity_to_degrees(float speed, float radial_velocity);


/**
 * @brief Calculate sweep frequency for a configuration
 *
 * @param[in] configuration Obstacle detector configuration
 * @return Sweep frequency
 */
float acc_detector_obstacle_processing_calculate_sweep_frequency(acc_detector_obstacle_configuration_t configuration);


/**
 * @}
 */

#endif
