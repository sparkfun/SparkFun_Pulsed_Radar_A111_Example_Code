// Copyright (c) Acconeer AB, 2019
// All rights reserved

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_board.h"
#include "acc_definitions.h"
#include "acc_device.h"
#include "acc_device_gpio.h"
#include "acc_device_os.h"
#include "acc_device_spi.h"

#if defined(TARGET_OS_linux)
#include "acc_driver_gpio_linux_sysfs.h"
#include "acc_driver_os_linux.h"
#include "acc_driver_spi_linux_spidev.h"
#else
#error "Target operating system is not supported"
#endif

#define PIN_HIGH (1)
#define PIN_LOW  (0)

#define SENSOR_COUNT (1)               /**< @brief The number of sensors available on the board */

#define PIN_SENSOR_INTERRUPT (25)      /**< @brief Gpio Interrupt Sensor BCM:25 J5:22, connect to sensor GPIO 5 */
#define PIN_SENSOR_ENABLE    (27)      /**< @brief SPI Sensor enable BCM:27 J5:13 */
#define PIN_SPI_SS_N         (8)       /**< @brief SPI SSn BCM:8 J5:24 */

#define ACC_BOARD_REF_FREQ  (26000000) /**< @brief The reference frequency assumes 26 MHz on reference board */
#define ACC_BOARD_SPI_SPEED (15000000) /**< @brief The SPI speed of this board */
#define ACC_BOARD_BUS       (0)        /**< @brief The SPI bus of this board */
#define ACC_BOARD_CS        (0)        /**< @brief The SPI device of the board */

/**
 * @brief Number of GPIO pins
 */
#define GPIO_PIN_COUNT 28

/**
 * @brief Sensor states
 */
typedef enum
{
	SENSOR_DISABLED,
	SENSOR_ENABLED,
} acc_board_sensor_state_t;


static acc_board_sensor_state_t sensor_state = SENSOR_DISABLED;

static acc_device_handle_t spi_handle;
static gpio_t              gpios[GPIO_PIN_COUNT];
static acc_os_semaphore_t  isr_semaphore;


static void isr_sensor(void)
{
	acc_os_semaphore_signal_from_interrupt(isr_semaphore);
}


static bool setup_isr(void)
{
	isr_semaphore = acc_os_semaphore_create();

	if (isr_semaphore == NULL)
	{
		return false;
	}

	if (!acc_device_gpio_register_isr(PIN_SENSOR_INTERRUPT, ACC_DEVICE_GPIO_EDGE_RISING, isr_sensor))
	{
		return false;
	}

	return true;
}


static void deinit(void)
{
	if (isr_semaphore != NULL)
	{
		acc_os_semaphore_destroy(isr_semaphore);
	}
}


bool acc_board_gpio_init(void)
{
	static bool init_done = false;

	if (init_done)
	{
		return true;
	}

	acc_os_init();

	/*
	   NOTE:
	                                PIN_SENSOR_INTERRUPT is active high (therefore PULL DOWN)
	                                PIN_SENSOR_ENABLE is active high (therefore PULL DOWN)
	                                PIN_SPI_SS_N is active low (therefore PULL UP)
	 */
	acc_device_gpio_set_initial_pull(PIN_SENSOR_INTERRUPT, PIN_LOW);
	acc_device_gpio_set_initial_pull(PIN_SENSOR_ENABLE, PIN_LOW);
	acc_device_gpio_set_initial_pull(PIN_SPI_SS_N, PIN_HIGH);

	/*
	   NOTE:
	                                PIN_SENSOR_INTERRUPT is active high (init state is LOW)
	                                PIN_SENSOR_ENABLE is active high (init state is LOW)
	                                PIN_SPI_SS_N is active low (init state is HIGH)
	 */
	if (!acc_device_gpio_write(PIN_SENSOR_ENABLE, PIN_LOW) ||
	    !acc_device_gpio_write(PIN_SPI_SS_N, PIN_HIGH) ||
	    !acc_device_gpio_input(PIN_SENSOR_INTERRUPT))
	{
		return false;
	}

	init_done = true;

	return true;
}


bool acc_board_init(void)
{
	static bool init_done = false;

	if (init_done)
	{
		return true;
	}

	acc_driver_os_linux_register();

	acc_os_init();

	acc_driver_gpio_linux_sysfs_register(GPIO_PIN_COUNT, gpios);
	acc_driver_spi_linux_spidev_register();

	acc_device_gpio_init();

	acc_device_spi_configuration_t configuration;

	configuration.bus           = ACC_BOARD_BUS;
	configuration.configuration = NULL;
	configuration.device        = ACC_BOARD_CS;
	configuration.master        = true;
	configuration.speed         = ACC_BOARD_SPI_SPEED;

	spi_handle = acc_device_spi_create(&configuration);

	if (!setup_isr())
	{
		deinit();
		return false;
	}

	init_done = true;

	return true;
}


void acc_board_start_sensor(acc_sensor_id_t sensor)
{
	(void)sensor;

	if (sensor_state != SENSOR_DISABLED)
	{
		return;
	}

	if (!acc_device_gpio_write(PIN_SENSOR_ENABLE, PIN_HIGH))
	{
		fprintf(stderr, "%s: Unable to activate enable_pin for sensor.\n", __func__);
		return;
	}

	acc_os_sleep_us(5000);

	// Clear pending interrupts
	while (acc_os_semaphore_wait(isr_semaphore, 0));

	sensor_state = SENSOR_ENABLED;
}


void acc_board_stop_sensor(acc_sensor_id_t sensor)
{
	(void)sensor;

	if (sensor_state != SENSOR_DISABLED)
	{
		// Disable sensor
		if (!acc_device_gpio_write(PIN_SENSOR_ENABLE, PIN_LOW))
		{
			// Set the state to enabled since it is not selected and failed to disable
			sensor_state = SENSOR_ENABLED;
			fprintf(stderr, "%s: Unable to deactivate enable_pin for sensor.\n", __func__);
			return;
		}

		sensor_state = SENSOR_DISABLED;
	}
}


uint32_t acc_board_get_sensor_count(void)
{
	return SENSOR_COUNT;
}


bool acc_board_wait_for_sensor_interrupt(acc_sensor_id_t sensor_id, uint32_t timeout_ms)
{
	(void)sensor_id;

	return acc_os_semaphore_wait(isr_semaphore, timeout_ms);
}


float acc_board_get_ref_freq(void)
{
	return ACC_BOARD_REF_FREQ;
}


void acc_board_sensor_transfer(acc_sensor_id_t sensor_id, uint8_t *buffer, size_t buffer_length)
{
	(void)sensor_id;

	uint_fast8_t bus = acc_device_spi_get_bus(spi_handle);

	acc_device_spi_lock(bus);

	if (!acc_device_spi_transfer(spi_handle, buffer, buffer_length))
	{
		// Error handling? (nothing done in other board files)
	}

	acc_device_spi_unlock(bus);
}
