// Copyright (c) Acconeer AB, 2019
// All rights reserved

#ifndef ACC_DETECTOR_OBSTACLE_CONFIGURATION_H_
#define ACC_DETECTOR_OBSTACLE_CONFIGURATION_H_

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#include "acc_definitions.h"


/**
 * @addtogroup Obstacle
 *
 * @brief Obstacle detector configuration API description
 *
 * @{
 */


/**
 * @brief Obstacle detector configuration container
 */
struct acc_detector_obstacle_configuration;

typedef struct acc_detector_obstacle_configuration *acc_detector_obstacle_configuration_t;


/**
 * @brief Background cancellation mode
 *
 * Coherent background: This method uses background cancellation per sweep, i.e., an average
 *                      complex background sweep is substracted from each sweep collected before
 *                      running the FFT.
 * Non-coherent background: This method uses background cancellation on the calculated FFT
 *                          envelope, i.e., an average FFT envelope is substracted from each
 *                          running FFT.
 *
 * @note: The background estimation is performed in accordance with the selected cancellation mode
 */
typedef enum
{
	ACC_DETECTOR_OBSTACLE_CONFIGURATION_BACKGROUND_CANCELLATION_MODE_COHERENT = 0,
	ACC_DETECTOR_OBSTACLE_CONFIGURATION_BACKGROUND_CANCELLATION_MODE_NON_COHERENT,
} acc_detector_obstacle_configuration_background_cancellation_mode_t;


/**
 * @brief Threshold parameters for non coherent background cancellation
 *
 * @param stationary The amplitude limit for stationary objects close to the sensor
 * @param moving The amplitude limit for moving objects and objects far from the sensor
 * @param distance_limit_far For distances larger than the far limit, use the @ref moving threshold
 * @param close_addition The amplitude increase at closest distance, applied for all velocities
 * @param distance_limit_near @ref close_addition threshold is applied from first distance in the range and linearly falls off to 0 at first distance plus distance_limit_near
 */
typedef struct
{
	float stationary;
	float moving;
	float distance_limit_far;
	float close_addition;
	float distance_limit_near;
} acc_detector_obstacle_configuration_threshold_t;


/**
 * @brief A callback for retrieving the FFT matrix used in the detector algorithm
 *
 * The FFT matrix is reported row by row starting with the top row. This row represents the area
 * farthest away from the sensor.
 *
 * @param[in] matrix_row A pointer to a row in the matrix
 * @param row_length The length of the row. The row length never varies for a created detector.
 * @param rows_left Rows left to be reported within the same matrix
 */
typedef void (*acc_detector_obstacle_probe_t)(const float complex *matrix_row, uint_fast16_t row_length, uint_fast16_t rows_left);


/**
 * @brief Create a configuration for an obstacle detector.
 *
 * @return A default configuration, if creation was not possible, a nullpointer is returned.
 */
acc_detector_obstacle_configuration_t acc_detector_obstacle_configuration_create(void);


/**
 * @brief Set the max speed of the carrier of the detector
 *
 * @param[in, out] obstacle_configuration The configuration to set the max speed for
 * @param[in] speed The max speed in meter per second
 * @param[in] rescale_highpass_speed The speed_highpass_mask is updated in proportion to the change of
 *            the speed if this parameter is true and the previous speed setting is nonzero. Otherwise,
 *            speed_highpass_mask is unaltered.
 */
void acc_detector_obstacle_configuration_set_max_speed(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                       float speed, bool rescale_highpass_speed);


/**
 * @brief Get the max speed of the carrier of  the detector
 *
 * @param[in] obstacle_configuration The configuration to get the max speed for
 *
 * @return The maximum speed
 */
float acc_detector_obstacle_configuration_get_max_speed(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set if the detector should be able to measure angle for sensors placed backwards compared to speed
 *
 * @param[in] obstacle_configuration The configuration to get the allow_reverse for
 * @param[in] allow_reverse True, enable to measure angle of objects moving away from sensor.
 *            False, enable for higher speed at lower sweep frequency
 */
void acc_detector_obstacle_configuration_set_allow_reverse(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                           bool                                  allow_reverse);


/**
 * @brief Get allow reverse status
 *
 * @param[in] obstacle_configuration The configuration
 *
 * @return Allow reverse status
 */
bool acc_detector_obstacle_configuration_get_allow_reverse(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set the highpass masking for radial speed.
 *
 * Objects that have a radial speed towards the sensor lower than he highpass masking
 * speed are not reported in the output. A zero input value deactivates the filter.
 *
 * The filter mask value represents the center of a maximally sharp transition from zero output
 * amplitude to unchanged ouput amplitude.
 *
 * @param[in] obstacle_configuration The configuration to set filter cutoff for
 * @param[in] speed_highpass_mask The mask transition value as radial speed in meter per second.
 *
 */
void acc_detector_obstacle_configuration_set_speed_filter_highpass(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                                   float                                 speed_highpass_mask);


/**
 * @brief Get the highpass filter cutoff for radial speed.
 *
 * Objects that have a radial speed towards the sensor lower than he highpass masking
 * speed are not reported in the output. A zero input value deactivates the filter.
 *
 * The filter mask value represents the center of a maximally sharp transition from zero output
 * amplitude to unchanged ouput amplitude.
 *
 * @param[in] obstacle_configuration The configuration to get the frequency for
 *
 * @return The current filter cutoff value as radial speed in meter per second. A zero value means
 *         that the filter is deactivated.
 */
float acc_detector_obstacle_configuration_get_speed_filter_highpass(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set object thresholds
 *
 * Set thresholds to distinguish objects from noise. The thresholds are applied after
 * background cancellation.
 *
 * @note
 *
 * @param[in] obstacle_configuration The configuration to set the thresholds for
 * @param[in] thresholds The threshold settings, see @ref acc_detector_obstacle_configuration_threshold_t
 */
void acc_detector_obstacle_configuration_set_thresholds(acc_detector_obstacle_configuration_t                 obstacle_configuration,
                                                        const acc_detector_obstacle_configuration_threshold_t *thresholds);


/**
 * @brief Get thresholds for non-coherent background cancellation mode
 *
 * see @ref acc_detector_obstacle_configuration_set_thresholds
 *
 * @param[in] obstacle_configuration The configuration to get the threshold for
 *
 * @return The thresholds, see @ref acc_detector_obstacle_configuration_threshold_t
 */
acc_detector_obstacle_configuration_threshold_t acc_detector_obstacle_configuration_get_thresholds(
	const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set the receiver gain in the radar sensor
 *
 * @param[in] obstacle_configuration The configuration to set the gain
 * @param gain The gain to set
 */
extern void acc_detector_obstacle_configuration_set_gain(acc_detector_obstacle_configuration_t obstacle_configuration, float gain);


/**
 * @brief Get the receiver gain in the radar sensor
 *
 * @param[in] obstacle_configuration The configuration to get the gain
 *
 * @return The gain
 */
extern float acc_detector_obstacle_configuration_get_gain(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set the start of the detection range
 *
 * The detector will not detect objects closer to the sensor than this range.
 *
 * @param[in] obstacle_configuration The configuration to set the range_start for
 * @param range_start The start of the range in meter
 */
void acc_detector_obstacle_configuration_set_range_start(acc_detector_obstacle_configuration_t obstacle_configuration, float range_start);


/**
 * @brief Get the start of the detection range
 *
 * The detector will not detect objects closer to the sensor than this range.
 *
 * @param[in] obstacle_configuration The configuration to get the range_start for
 *
 * @return The start of the range in meter
 */
float acc_detector_obstacle_configuration_get_range_start(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set the length of the detection range counted from range start
 *
 * The detector will not detect objects beyond this range.
 *
 * @param[in] obstacle_configuration The configuration to set the range_end for
 * @param range_length The length of the range in meter
 */
void acc_detector_obstacle_configuration_set_range_length(acc_detector_obstacle_configuration_t obstacle_configuration, float range_length);


/**
 * @brief Get the length of the detection range counted from range start
 *
 * The detector will not detect objects beyond this range.
 *
 * @param[in] obstacle_configuration The configuration to get the range_end for
 *
 * @return The length of the range in meter
 */
float acc_detector_obstacle_configuration_get_range_length(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set the length of the sweep range beyond the end of the obstacle detection range
 *
 * There is a need for extending the sweep range slightly beyond the obstacle detection for detection
 * of obstacles close the end of the obstacle detection range. The sweep end overscan parameter sets
 * added sweep length.
 *
 * @param[in] obstacle_configuration The configuration to set the range_end for
 * @param[in] range_end_overscan The range end overscan in meter
 */
void acc_detector_obstacle_configuration_set_range_end_overscan(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                                float                                 range_end_overscan);


/**
 * @brief Get the length of the sweep range beyond the end of the obstacle detection range
 *
 * There is a need for extending the sweep range slightly beyond the obstacle detection for detection
 * of obstacles close the end of the obstacle detection range. The sweep end overscan parameter sets
 * added sweep length.
 *
 * @param[in] obstacle_configuration The configuration to get the range_end for
 *
 * @return The range end overscan in meter
 */
float acc_detector_obstacle_configuration_get_range_end_overscan(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set the sensor ID on the sensor the detector should run on
 *
 * @param[in] obstacle_configuration The configuration to set the sensor for
 * @param sensor The sensor id to set
 */
void acc_detector_obstacle_configuration_set_sensor(acc_detector_obstacle_configuration_t obstacle_configuration, acc_sensor_id_t sensor);


/**
 * @brief Get the sensor ID
 *
 * @param[in] obstacle_configuration The configuration to get the sensor for
 *
 * @return The sensor id
 */
acc_sensor_id_t acc_detector_obstacle_configuration_get_sensor(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set the background cancellation mode
 *
 * See @ref acc_detector_obstacle_configuration_background_cancellation_mode_t
 *
 * @param[in] obstacle_configuration The configuration to set the the background cancellation mode for
 * @param[in] mode TBD
 */
void acc_detector_obstacle_configuration_set_background_cancellation_mode(
	acc_detector_obstacle_configuration_t                              obstacle_configuration,
	acc_detector_obstacle_configuration_background_cancellation_mode_t mode);


/**
 * @brief Get the background cancellation mode
 *
 * See @ref acc_detector_obstacle_configuration_background_cancellation_mode_t
 *
 * @param[in] obstacle_configuration The configuration to set the the background cancellation mode for
 */
acc_detector_obstacle_configuration_background_cancellation_mode_t acc_detector_obstacle_configuration_get_background_cancellation_mode(
	const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set the number of iterations that should be done for background estimation
 *
 * @param[in] obstacle_configuration The configuration to set the the number of background estimation iterations for
 * @param[in] iterations The number of background estimation iterations to use
 */
void acc_detector_obstacle_configuration_set_background_estimation_iterations(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                                              uint16_t                              iterations);


/**
 * @brief Get the number of iterations that should be collected for background estimation
 *
 * @param[in] obstacle_configuration The configuration
 *
 * @return The number of background estimation iterations to use
 */
uint16_t acc_detector_obstacle_configuration_get_background_estimation_iterations(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set the scale factor used in the noncoherent background estimation
 *
 * @param[in] obstacle_configuration The configuration that the setting shall be applied to
 * @param background_scale The value to be used for noncoherent background estimation
 */
void acc_detector_obstacle_configuration_set_background_scale(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                              float                                 background_scale);


/**
 * @brief Get the scale factor used in the noncoherent background estimation
 *
 * @param[in] obstacle_configuration The configuration
 *
 * @return The value to be used in noncoherent background estimation
 */
float acc_detector_obstacle_configuration_get_background_scale(acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set downsample scale factor
 *
 * A higher downsample scale will downsample the data more.
 * A high downsample scale will result in less memory usage at resolution cost.
 *
 * @param[in] obstacle_configuration The configuration to set the sweep downsample scale for
 * @param sweep_down_sample_scale The sweep downsample scale
 */
void acc_detector_obstacle_configuration_set_sweep_downsample_scale(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                                    uint16_t                              sweep_down_sample_scale);


/**
 * @brief Get downsample scale factor
 *
 * see acc_detector_obstacle_configuration_set_sweep_downsample_scale
 *
 * @param[in] obstacle_configuration The configuration
 *
 * @return sweep_down_sample_scale The sweep downsample scale
 */
uint16_t acc_detector_obstacle_configuration_get_sweep_downsample_scale(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set max number of obstacles
 *
 * The detector will never detect more obstacles than max_number_of_obstacles
 *
 * @param[in] obstacle_configuration The configuration to set the max number of obstacles for
 * @param max_number_of_obstacles The max number of obstacles to be detected
 */
void acc_detector_obstacle_configuration_set_max_number_of_obstacles(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                                     uint16_t                              max_number_of_obstacles);


/**
 * @brief Get max number of obstacles
 *
 * The detector will never detect more obstacles than max_number_of_obstacles
 *
 * @param[in] obstacle_configuration The configuration to set the max number of obstacles for
 *
 * @return The max number of obstacles to be detected
 */
extern uint16_t acc_detector_obstacle_configuration_get_max_number_of_obstacles(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set static distance offset
 *
 * This offset represents an empirically determined static distance erro. It is subtracted from the
 * reported distance for every obstacle.
 *
 * @param[in] obstacle_configuration The configuration to set the distance offset for
 * @param distance_offset The offset in meters to be used
 */
void acc_detector_obstacle_configuration_set_distance_offset(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                             float                                 distance_offset);


/**
 * @brief Get static obstacle distance offset
 *
 * This offset represents an empirically determined static distance erro. It is subtracted from the
 * reported distance for every obstacle.
 *
 * @return The offset in meters to be added to reported obstacle distances
 */
float acc_detector_obstacle_configuration_get_distance_offset(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set the edge detection level ratio for the obstacle distance calculation
 *
 * This configuration parameter controls the level used for edge detection for a found obstacle.
 * The value should be in the range from 0 to 1. The obstacle distance is calculated from the
 * near edge of the peak where the signal amplitude crosses a threshold. This threshold is given
 * by the ratio parameter times the peak amplitude unless this product is lower than the detection
 * threshold set by @ref acc_detector_obstacle_configuration_set_thresholds or
 * @ref acc_detector_obstacle_configuration_set_thresholds. In the latter case,
 * the detection threshold is used. A zero value of the ratio parameter ignores the peak level and
 * the reported peak distance tells the position of the near edge of the peak where the amplitude
 * crosses the peak detection threshold. A value of 1 cuts the edge-detection procedure short and
 * reports the peak position.
 *
 * @param[in] obstacle_configuration The configuration that the setting shall be applied to
 * @param edge_to_peak_ratio The value to be used for the edge to peak level ratio
 */
void acc_detector_obstacle_configuration_set_edge_to_peak_ratio(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                                float                                 edge_to_peak_ratio);


/**
 * @brief Get the edge detection level ratio
 *
 * See @ref acc_detector_obstacle_configuration_set_edge_to_peak_ratio for a functional
 * description of the returned parameter.
 *
 * @return The value of the edge detection level ratio
 */
float acc_detector_obstacle_configuration_get_edge_to_peak_ratio(acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Set FFT probe
 *
 * Set to NULL if not needed
 *
 * @param[in] obstacle_configuration The configuration to set the fft probe for
 * @param fft_probe The fft probe to set
 */
void acc_detector_obstacle_configuration_set_fft_probe(acc_detector_obstacle_configuration_t obstacle_configuration,
                                                       acc_detector_obstacle_probe_t         fft_probe);


/**
 * @brief Get FFT probe
 *
 * see acc_detector_obstacle_configuration_set_fft_probe
 *
 * @param[in] obstacle_configuration The configuration
 *
 * @return The fft probe
 */
acc_detector_obstacle_probe_t acc_detector_obstacle_configuration_get_fft_probe(const acc_detector_obstacle_configuration_t obstacle_configuration);


/**
 * @brief Destroy a configuration for an obstacle detector.
 *
 * @param[in] obstacle_configuration The configuration to destroy
 */
void acc_detector_obstacle_configuration_destroy(acc_detector_obstacle_configuration_t *obstacle_configuration);


/**
 * @}
 */

#endif
