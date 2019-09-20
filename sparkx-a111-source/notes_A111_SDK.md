Acconeer Software Development Kit for the A111 Pulsed Radar
------------------------------------

## Types

* **uint_fast8_t** - the _fastest_ unsigned integer byte with at _least_ 8 bits
  * Fast **how**? - This, I think, comes into play with the compiler as it will
    then be able to choose the fastest type for the given operation. 
  * This comes down to how **compilers** type define for stdint.h types. They
    can use long or _int_ as typedef for int32_t for example! 
  * So when uint_fast8_t is used, it's up to the compiler to choose the
    _fastest_ type for the operation. 

* typedef and "_t_"
  * This is a convention begun (and _still_ used) with C for types defined by **typdef**.

* **static** identifier - data description
  * Mantains the modified variable outside of local scope of a function
    * It accomplishes this at a low level by making space in memory for the
      variable on **compilation** rather than automatically when the function is
      **called**. 
  * If used on a global variable, the scope of the variable or function is
    contained to the file.

## Notes on Library
* Author checked on status of the pulsed radar often, **specifically** whether
  it was busy by accessing `sensor_state` Arr. 
  #### **Example** 
  ```C
  if (sensor_state[sensor-1] != SENSOR_STATE_BUSY) 
  ```
* `return`-ing the result from function by calling function on `return` 
  * _object oriented_

## File Structure of Example Code 
* **acc-board-rpi-sparkx.c** - Function describing how to reset the board,
  checking when the board is busy, initializing the board, getting sensor
  count, getting frequency of the sensor etc.
    * The **SETUP**
* **sparkx-detector-distance.c** - Example code showing how to use the _distance
  peak detector_.
    * **distance peak detector** - notes directly from the script: 
    ```C
     * The example executes as follows:
     *   - Activate Radar System Services (RSS)
     *   - Create a distance peak detector configuration (with blocking mode as default)
     *   - Create a distance peak detector using the previously created configuration
     *   - Activate the distance peak detector
     *   - Get the result and print it 100 times
     *   - Deactivate and destroy the distance peak detector
     *   - Destroy the distance peak detector configuration
     *   - Deactivate Radar System Services
     */
    ``` 
      * Already here we're seeing static to keep scope of functions to local
        file (_permissions_)?

## Comparing the current SDK with provided SparkX files. 
* Number of linking source files that no longer exist: 
    ```C
    libacconeer_a111_r2c.c
    libacc_local_server.c
    libacc_message_driver_dummy.c
    libacc_envelope.c
    libacc_power_bins.c
    ```
    * What do I do with libcustomer.c?

* Number of header files that changed names: 
  ```C
  acc_os -> acc_device_os
  ```

* Board hardware definition: 
  * Need `CE_A_PIN` or `CE_B_PIN`? 
  * `acc_board_gpio_init` has changed to return `bool` rather than `acc_status_t`
    * Need to change throughout

## HAL - Hardware Annotation Library
