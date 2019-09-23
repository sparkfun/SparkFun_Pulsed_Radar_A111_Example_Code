// Copyright (c) Acconeer AB, 2017-2018
// All rights reserved

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "acc_board.h"
#include "acc_definitions.h" // - added for acc_os_mutex_t
#include "acc_device_gpio.h"
#include "acc_device_os.h"
#include "acc_device_spi.h"
#include "acc_driver_gpio_linux_sysfs.h"
#include "acc_driver_os_linux.h"
#include "acc_driver_spi_linux_spidev.h"
#include "acc_log.h"

/**
 * @brief The module name
 *
 * Must exist if acc_log.h is used.
 */
#define MODULE	"acc_board_rpi_sparkfun"

/**
 * @brief The number of sensors available on the board
 */
#define SENSOR_COUNT	1

/**
 * @brief Host GPIO pin number (BCM)
 *
 * This GPIO should be connected to sensor 1 GPIO 5 
 */
#define GPIO0_PIN	25      // Interrupt pin

/**
 * @brief Host GPIO pin number (BCM)
 */
/**@{*/
#define RSTn_PIN	6        // Not connected
#define ENABLE_PIN	27

//#define CE_PIN      8        // Breakout CS pin
/**@}*/

/**
 * @brief The reference frequency used by this board
 *
 * This assumes 24 MHz on XR111-3 R1C
 */
#define ACC_BOARD_REF_FREQ	26000000

/**
 * @brief The SPI speed of this board
 */
//#define ACC_BOARD_SPI_SPEED	15000000
#define ACC_BOARD_SPI_SPEED	1000000

/**
 * @brief The SPI bus all sensors are using
 */
#define ACC_BOARD_BUS		0

/**
 * @brief The SPI CS all sensors are using
 */
#define ACC_BOARD_CS		0


/**
 * @brief Sensor states
 */
typedef enum {
	SENSOR_STATE_UNKNOWN,
	SENSOR_STATE_READY,
	SENSOR_STATE_BUSY
} acc_board_sensor_state_t;


/**
 * @brief Sensor state collection that keeps track of each sensor's current state
 */
static acc_board_sensor_state_t sensor_state[SENSOR_COUNT] = {SENSOR_STATE_UNKNOWN};

static acc_device_handle_t spi_handle;
static acc_os_semaphor_t isr_semaphores[SENSOR_COUNT];

static void isr_sensor1(void)

{
	acc_os_semaphore_signal_from_interrupt(isr_semaphores[0]);
}

static bool setup_isr(void)
{
	for (uint_fast8_t i = 0; i < SENSOR_COUNT; i++)
	{
		isr_semaphores[i] = acc_os_semaphore_create();

		if (isr_semaphores[i] == NULL)
		{
			return false;
		}
	}

	if ( !acc_device_gpio_register_isr(GPIO0_PIN, ACC_DEVICE_GPIO_EDGE_RISING, &isr_sensor1)){
		return false;
	}

	return true;
}


static void deinit(void)
{
	for (uint_fast8_t i = 0; i < SENSOR_COUNT; i++)
	{
		if (isr_semaphores[i] != NULL)
		{
			acc_os_semaphore_destroy(isr_semaphores[i]);
		}
	}
}

/**
 * @brief Get the combined status of all sensors
 *
 * @return False if any sensor is busy
 */
static bool acc_board_all_sensors_inactive(void)
{
	for (uint_fast8_t sensor_index = 0; sensor_index < SENSOR_COUNT; sensor_index++) {
		if (sensor_state[sensor_index] == SENSOR_STATE_BUSY) {
			return false;
		}
	}
	return true;
}


bool acc_board_gpio_init(void)
{
	static bool		init_done = false;

	if (init_done) {
		return true;
	}

	acc_os_init();

	acc_device_gpio_set_initial_pull(GPIO0_PIN, 0);
	acc_device_gpio_set_initial_pull(RSTn_PIN, 1);
	acc_device_gpio_set_initial_pull(ENABLE_PIN, 0);

	if(
		!acc_device_gpio_input(GPIO0_PIN))  ||
		!acc_device_gpio_write(RSTn_PIN, 0) ||
		!acc_device_gpio_write(ENABLE_PIN, 0))
	{
		ACC_LOG_WARNING("%s: failed to set initial pull with status: %s", __func__, false);
	  return false;
	}

	init_done = true;

	return true;
}


bool acc_board_init(void)
{
	static bool		init_done = false;

	if (init_done) {
		return true;
	}

	acc_driver_os_linux_register();
	acc_os_init();

	acc_driver_spi_linux_spidev_register();
  acc_device_gpio_init();

  acc_device_spi_configuration_t configuration;

  configuration.bus           = ACC_BOARD_BUS;
  configuration.configuration = NULL;
  configuration.device        = ACC_BOARD_CS;
  configuration.master        = true;
  configuration.speed         = ACC_BOARD_SPI_SPEED;

  spi_handle = acc_device_spi_create(&configuration);

	for (uint_fast8_t sensor_index = 0; sensor_index < SENSOR_COUNT; sensor_index++) {
		sensor_state[sensor_index] = SENSOR_STATE_UNKNOWN;
	}

	init_done = trueacc_device_gpio_input;
	acc_os_mutex_unlock(init_mutex);

	return true;
}


/**
 * @brief Reset sensor
 *
 * Default setup when sensor is not active
 *
 * @return Status
 */
static bool acc_board_reset_sensor(void)
{

	status = acc_device_gpio_write(RSTn_PIN, 0);
	if (status != true) {
		ACC_LOG_ERROR("Unable to activate RSTn");
		return status;
	}

	status = acc_device_gpio_write(ENABLE_PIN, 0);
	if (status != true) {
		ACC_LOG_ERROR("Unable to deactivate ENABLE");
		return status;
	}

	return true;
}


bool acc_board_start_sensor(acc_sensor_t sensor)
{

	if (sensor_state[sensor - 1] == SENSOR_STATE_BUSY) {
		ACC_LOG_ERROR("Sensor %u already active.", sensor);
		return false;
	}

	if (acc_board_all_sensors_inactive()) {
		status = acc_device_gpio_write(RSTn_PIN, 0);
		if (status != true) {
			ACC_LOG_ERROR("Unable to activate RSTn");
			acc_board_reset_sensor();
			return status;
		}

		status = acc_device_gpio_write(ENABLE_PIN, 1);
		if (status != true) {
			ACC_LOG_ERROR("Unable to activate ENABLE");
			acc_board_reset_sensor();
			return status;
		}

		// Wait for Power On Reset
		acc_os_sleep_us(5000);

		status = acc_device_gpio_write(RSTn_PIN, 1);
		if (status != true) {
			ACC_LOG_ERROR("Unable to deactivate RSTn");
			acc_board_reset_sensor();
			return status;
		}

		for (uint_fast8_t sensor_index = 0; sensor_index < SENSOR_COUNT; sensor_index++) {
			sensor_state[sensor_index] = SENSOR_STATE_READY;
		}
	}

	if (sensor_state[sensor - 1] != SENSOR_STATE_READY) {
		ACC_LOG_ERROR("Sensor has not been reset");
		return false;
	}

	sensor_state[sensor - 1] = SENSOR_STATE_BUSY;

	return true;
}


bool acc_board_stop_sensor(acc_sensor_t sensor)
{
	if (sensor_state[sensor - 1] != SENSOR_STATE_BUSY) {
		ACC_LOG_ERROR("Sensor %u already inactive.", sensor);
		return false;
	}

	sensor_state[sensor - 1] = SENSOR_STATE_UNKNOWN;

	if (acc_board_all_sensors_inactive()) {
		return acc_board_reset_sensor();
	}

	return true;
}


void acc_board_get_spi_bus_cs(acc_sensor_t sensor, uint_fast8_t *bus, uint_fast8_t *cs)
{
	if ((sensor <= 0) || (sensor > SENSOR_COUNT)) {
		*bus = -1;
		*cs  = -1;
	} else {
		*bus = ACC_BOARD_BUS;
		*cs  = ACC_BOARD_CS;
	}
}


bool acc_board_chip_select(acc_sensor_t sensor, uint_fast8_t cs_assert)
{
	ACC_UNUSED(sensor);
	ACC_UNUSED(cs_assert);

	if (cs_assert) {
		uint_fast8_t cea_val = (sensor == 1 || sensor == 2) ? 0 : 1;
		uint_fast8_t ceb_val = (sensor == 1 || sensor == 3) ? 0 : 1;

		if (
			(status = acc_device_gpio_write(CE_A_PIN, cea_val)) ||
			(status = acc_device_gpio_write(CE_B_PIN, ceb_val))
		) {
			ACC_LOG_ERROR("%s failed with %s", __func__, acc_log_status_name(status));
			return status;
		}
		status = acc_device_gpio_write(CE_PIN, 0);
		if (status) {
			ACC_LOG_ERROR("%s failed with %s", __func__, acc_log_status_name(status));
			return status;
		}
	}*/

	return true;
}


acc_sensor_t acc_board_get_sensor_count(void)
{
	return SENSOR_COUNT;
}


bool acc_board_is_sensor_interrupt_connected(acc_sensor_t sensor)
{
	ACC_UNUSED(sensor);

	return false;
}


bool acc_board_is_sensor_interrupt_active(acc_sensor_t sensor)
{
	uint_fast8_t value;

	status = acc_device_gpio_read(sensor_interrupt_pins[sensor - 1], &value);
	if (status != true) {
		ACC_LOG_ERROR("Could not obtain GPIO interrupt value for sensor %" PRIsensor " with status: %s.", sensor, acc_log_status_name(status));
		return false;
	}

	return value != 0;
}


float acc_board_get_ref_freq(void)
{
	return ACC_BOARD_REF_FREQ;
}


uint32_t acc_board_get_spi_speed(uint_fast8_t bus)
{
	ACC_UNUSED(bus);

	return ACC_BOARD_SPI_SPEED;
}


bool acc_board_set_ref_freq(float ref_freq)
{
	ACC_UNUSED(ref_freq);

	return ACC_STATUS_UNSUPPORTED;
}
