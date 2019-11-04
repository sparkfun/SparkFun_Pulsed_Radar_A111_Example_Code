// Copyright (c) Acconeer AB, 2015-2019
// All rights reserved

#ifndef ACC_BOARD_H_
#define ACC_BOARD_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "acc_definitions.h"


typedef void (*acc_board_isr_t)(acc_sensor_id_t);


/**
 * @brief Default SPI speed
 */
#define ACC_BOARD_DEFAULT_SPI_SPEED	5000000


/**
 * @brief Initialize board
 *
 * Should only be called from sensor_driver_init.
 *
 * @return True if successful, false otherwise
 */
extern bool acc_board_init(void);


/**
 * @brief Initialize the direction and level of host GPIOs
 *
 * Must be called after acc_board_init, otherwise the GPIO initialization will not succeed.
 *
 * @return True if successful, false otherwise
 */
extern bool acc_board_gpio_init(void);


/**
 * @brief Start sensor
 *
 * Setup in order to communicate with the specified sensor.
 *
 * @param[in] sensor The sensor to be started
 */
extern void acc_board_start_sensor(acc_sensor_id_t sensor);


/**
 * @brief Stop sensor
 *
 * Setup when not needing to communicate with the specified sensor.
 *
 * @param[in] sensor The sensor to be stopped
 */
extern void acc_board_stop_sensor(acc_sensor_id_t sensor);


/**
 * @brief Custom chip select for SPI transfer
 *
 * To be called from sensor_driver_transfer.
 *
 * @param[in] sensor The specific sensor
 * @param[in] cs_assert Chip select or deselect
 * @return True if successful, false otherwise
 */
extern bool acc_board_chip_select(acc_sensor_id_t sensor, uint_fast8_t cs_assert);


/**
 * @brief Wait for a sensor interrupt
 *
 * @param[in] sensor_id The sensor to wait for the interrupt on
 * @param[in] timeout_ms The maximum time to wait in milliseconds
 * @return True if an interrupt has occurred within timeout, false if timeout occurred
 */
extern bool acc_board_wait_for_sensor_interrupt(acc_sensor_id_t sensor_id, uint32_t timeout_ms);


/**
 * @brief Retrieves the number of sensors connected to the device
 *
 * @return The number of sensors
 */
extern uint32_t acc_board_get_sensor_count(void);


/**
 * @brief Retrieves the reference frequency of the clock supplied from the board
 *
 * @return The reference frequency
 */
extern float acc_board_get_ref_freq(void);


/**
 * @brief Inform which reference frequency the system is using
 *
 * @param[in] ref_freq Reference frequency
 * @return True if successful, false otherwise
 */
extern bool acc_board_set_ref_freq(float ref_freq);


/**
 * @brief Transfer data to/from sensor
 *
 * @param sensor_id The sensor to transfer to/from
 * @param buffer The data to be transferred
 * @param buffer_length The size of the buffer
 */
extern void acc_board_sensor_transfer(acc_sensor_id_t sensor_id, uint8_t *buffer, size_t buffer_length);

#endif
