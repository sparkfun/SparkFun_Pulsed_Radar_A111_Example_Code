// Copyright (c) Acconeer AB, 2018-2019
// All rights reserved

#ifndef ACC_DETECTOR_MOTION_H_
#define ACC_DETECTOR_MOTION_H_

#include <stdbool.h>

#include "acc_sweep_configuration.h"


/**
 * @defgroup Motion Motion Detector
 * @ingroup Detectors
 *
 * @brief Motion detector API description
 *
 * @deprecated Motion detector is deprecated and it is recommended to use the presence detector
 *
 * @{
 */


/**
 * @brief Motion detector configuration container
 */
struct acc_detector_motion_configuration;
typedef struct acc_detector_motion_configuration *acc_detector_motion_configuration_t;


/**
 * @brief Motion detector handle
 */
struct acc_detector_motion_handle;
typedef struct acc_detector_motion_handle *acc_detector_motion_handle_t;


/**
 * @brief Create a configuration for a motion detector
 *
 * @return Motion detector configuration, NULL if creation was not possible
 */
extern acc_detector_motion_configuration_t acc_detector_motion_configuration_create(void);


/**
 * @brief Destroy a motion detector configuration
 *
 * Destroy a motion detector configuration that is no longer needed, may be done even if a
 * motion detector has been created with the specific configuration and has not yet been destroyed.
 * The motion detector configuration reference is set to NULL after destruction.
 * If NULL is sent in, nothing happens.
 *
 * @param[in] motion_configuration The configuration to destroy, set to NULL
 */
extern void acc_detector_motion_configuration_destroy(acc_detector_motion_configuration_t *motion_configuration);


/**
 * @brief Create a motion detector with the provided configuration
 *
 * Only one motion detector may exist for a specific sensor at any given time and
 * invalid configurations will not allow for motion detector creation.
 *
 * @param[in] motion_configuration The motion detector configuration to create a motion detector with
 * @return Motion detector handle, NULL if motion detector was not possible to create
 */
extern acc_detector_motion_handle_t acc_detector_motion_create(acc_detector_motion_configuration_t motion_configuration);


/**
 * @brief Destroy a motion detector identified with the provided handle
 *
 * Destroy the context of a motion detector allowing another motion detector to be created using the
 * same resources. The motion detector handle reference is set to NULL after destruction.
 * If NULL is sent in, nothing happens.
 *
 * @param[in] motion_handle A reference to the motion detector handle to destroy
 */
extern void acc_detector_motion_destroy(acc_detector_motion_handle_t *motion_handle);


/**
 * @brief Activate the motion detector associated with the provided handle
 *
 * @param[in] motion_handle The motion detector handle for the motion detector to activate
 * @return True if successful, otherwise false
 */
extern bool acc_detector_motion_activate(acc_detector_motion_handle_t motion_handle);


/**
 * @brief Deactivate the motion detector associated with the provided handle
 *
 * @param[in] motion_handle The motion detector handle for the motion detector to deactivate
 * @return True if successful, otherwise false
 */
extern bool acc_detector_motion_deactivate(acc_detector_motion_handle_t motion_handle);


/**
 * @brief Checks if the detector is activated or not
 *
 * @param[in] motion_handle The motion detector handle to check
 * @return True if the detector is active, false otherwise
 */
extern bool acc_detector_motion_is_active(acc_detector_motion_handle_t motion_handle);


/**
 * @brief Retrieve the next result from the motion detector
 *
 * May only be called after a motion detector has been activated to retrieve the next result, blocks
 * the application until a result is ready.
 *
 * @param[in] motion_handle The motion detector handle for the motion detector to get the next result for
 * @param[out] motion_detected Motion detector result
 * @return True if successful, otherwise false
 */
extern bool acc_detector_motion_get_next(acc_detector_motion_handle_t motion_handle, bool *motion_detected);


/**
 * @brief Retrieve a sweep configuration from a motion detector configuration
 *
 * Do not set sensor_update_rate_hz in acc_sweep_configuration_repetition_mode_streaming_set(). The behaviour is then undefined.
 * Use acc_detector_motion_configuration_update_rate_set() instead.
 *
 * @param[in] motion_configuration The motion detector configuration to get a sweep configuration from
 * @return Sweep configuration, NULL if the motion detector configuration does not contain a sweep configuration or an error occured
 */
extern acc_sweep_configuration_t acc_detector_motion_configuration_get_sweep_configuration(acc_detector_motion_configuration_t motion_configuration);


/**
 * @brief Get sensitivity factor
 *
 * The sensitivity factor is a number between 0.0 and 1.0 where 0.0 is the least sensitive the detector can be
 * and 1.0 is the most sensitive the detector can be.
 *
 * @param[in] motion_configuration The configuration to get the sensitivity factor for
 * @return Sensitivity factor
 */
extern float acc_detector_motion_configuration_sensitivity_get(acc_detector_motion_configuration_t motion_configuration);


/**
 * @brief Set sensitivity factor
 *
 * The sensitivity factor is a number between 0.0 and 1.0 where 0.0 is the least sensitive the detector can be
 * and 1.0 is the most sensitive the detector can be.
 *
 * @param[in] motion_configuration The configuration to set the sensitivity factor for
 * @param[in] sensitivity_factor The sensitivity factor
 */
extern void acc_detector_motion_configuration_sensitivity_set(acc_detector_motion_configuration_t motion_configuration, float sensitivity_factor);


/**
 * @brief Get update rate
 *
 * The update rate, in Hz, sets how often the motion detector is expected to be asked to provide a result.
 * It is up to the calling application to make sure the detector is asked for a result at the given update rate.
 * Some internal filters are calculated from this update rate and the detector may not work as expected if
 * the actulal timing doesn't match.
 * It is a number larger than 0.0003.
 *
 * @param[in] motion_configuration The configuration to get the update rate for
 * @return Update rate, on failure it returns 0.0
 */
extern float acc_detector_motion_configuration_update_rate_get(acc_detector_motion_configuration_t motion_configuration);


/**
 * @brief Set update rate
 *
 * The update rate, in Hz, sets how often the motion detector is expected to be asked to provide a result.
 * It is up to the calling application to make sure the detector is asked for a result at the given update rate.
 * Some internal filters are calculated from this update rate and the detector may not work as expected if
 * the actulal timing doesn't match.
 * It is a number larger than 0.0003.
 *
 * @param[in] motion_configuration The configuration to set the update rate for
 * @param[in] update_rate The update rate
 */
extern void acc_detector_motion_configuration_update_rate_set(acc_detector_motion_configuration_t motion_configuration, float update_rate);


/**
 * @}
 */


#endif
