// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef OPENTITAN_SW_DEVICE_LIB_DIF_DIF_PINMUX_H_
#define OPENTITAN_SW_DEVICE_LIB_DIF_DIF_PINMUX_H_

/**
 * @file
 * @brief <a href="/hw/ip/pinmux/doc/">Pin Multiplexer</a> Device Interface Functions
 */

/**
 * Pin Multiplexer connects peripheral input/output signals to the padring MIO
 * input/output signals.
 *
 * Every peripheral input signal is fed into a multiplexer, where selects
 * determine which padring MIO input or constants should be connected to it.
 *
 * Every padring MIO output signal is fed into a multiplexer, where selects
 * determine which peripheral output or constants should be connected to it.
 */

#include <stdbool.h>
#include <stdint.h>

#include "sw/device/lib/base/mmio.h"
#include "sw/device/lib/dif/dif_warn_unused_result.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * A toggle state: enabled, or disabled.
 *
 * This enum may be used instead of a `bool` when describing an enabled/disabled
 * state.
 */
typedef enum dif_pinmux_toggle {
  /*
   * The "enabled" state.
   */
  kDifPinmuxToggleEnabled,
  /**
   * The "disabled" state.
   */
  kDifPinmuxToggleDisabled,
} dif_pinmux_toggle_t;

/**
 * Hardware instantiation parameters for Pin Multiplexer.
 *
 * This struct describes information about the underlying hardware that is
 * not determined until the hardware design is used as part of a top-level
 * design.
 */
typedef struct dif_pinmux_params {
  /**
   * The base address for the Pin Multiplexer hardware registers.
   */
  mmio_region_t base_addr;
} dif_pinmux_params_t;

// TODO
// dif_pinmux_padring_dio_t
// dif_pinmux_peripheral_input_t
// dif_pinmux_padring_insel_t
// dif_pinmux_peripheral_outsel_t

/**
 * TODO.
 */
typedef enum dif_pinmux_sleep_mode {
  kDifPinmuxSleepModeLow = 0,
  kDifPinmuxSleepModeHigh,
  kDifPinmuxSleepModeHighZ,
  kDifPinmuxSleepModeKeep,
} dif_pinmux_sleep_mode_t;

/**
 * TODO.
 */
typedef struct dif_pinmux_sleep_config {
  dif_pinmux_toggle_t enable;
  dif_pinmux_sleep_mode_t mode;
} dif_pinmux_sleep_config_t;

/**
 * A handle to Pin Multiplexer.
 *
 * This type should be treated as opaque by users.
 */
typedef struct dif_pinmux {
  dif_pinmux_params_t params;
} dif_pinmux_t;

/**
 * The result of a Pin Multiplexer operation.
 */
typedef enum dif_pinmux_result {
  /**
   * Indicates that the operation succeeded.
   */
  kDifPinmuxOk = 0,
  /**
   * Indicates some unspecified failure.
   */
  kDifPinmuxError = 1,
  /**
   * Indicates that some parameter passed into a function failed a
   * precondition.
   *
   * When this value is returned, no hardware operations occured.
   */
  kDifPinmuxBadArg = 2,
  /**
   * Indicates that this operation has been locked out, and can never
   * succeed until hardware reset.
   */
  kDifPinmuxLocked = 3,
} dif_pinmux_result_t;

/**
 * The result of a Pin Multiplexer operation.
 */
typedef enum dif_pinmux_init_result {
  /**
   * Indicates that the operation succeeded.
   */
  kDifPinmuxInitOk = kDifPinmuxOk,
  /**
   * Indicates some unspecified failure.
   */
  kDifPinmuxInitError = kDifPinmuxError,
  /**
   * Indicates that some parameter passed into a function failed a
   * precondition.
   *
   * When this value is returned, no hardware operations occurred.
   */
  kDifPinmuxInitBadArg = kDifPinmuxBadArg,
} dif_pinmux_init_result_t;

/**
 * Creates a new handle for Pin Multiplexer.
 *
 * This function does not actuate the hardware.
 *
 * @param params Hardware instantiation parameters.
 * @param[out] pinmux Out param for the initialized handle.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_init_result_t dif_pinmux_init(dif_pinmux_params_t params,
                                         dif_pinmux_t *pinmux);

/**
 * Sets the connection between a peripheral input and a Padring MIO input.
 *
 * `input` can be connected to any available Padring MIO input.
 *
 * @param pinmux A Pin Multiplexer handle.
 * @param input Peripheral input.
 * @param select Padring MIO input to be connected to `peripheral_input`.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_input_select(const dif_pinmux_t *pinmux,
                                            dif_pinmux_peripheral_input_t input,
                                            dif_pinmux_padring_insel_t select);

/**
 * Locks out Pin Multiplexer input select for a given peripheral input.
 *
 * This function is reentrant: calling it while functionality is locked will
 * have no effect and return `kDifPinmuxOk`.
 *
 * @param pinmux A Pin Multiplexer handle.
 * @param input Peripheral input.
 * @return `dif_pinmux_result_t`.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_input_select_lock(
    dif_pinmux_t *pinmux, dif_pinmux_peripheral_input_t input);

/**
 * Checks whether this Pin Multiplexer input select is locked.
 *
 * @param pinmux A Pin Multiplexer handle.
 * @param input Peripheral input.
 * @param[out] is_locked Out-param for the locked state.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_input_select_is_locked(
    const dif_pinmux_t *pinmux, dif_pinmux_peripheral_input_t input,
    bool *is_locked);

/**
 * Sets the connection between a Padring MIO output and peripheral output.
 *
 * `output` can be connected to any available `peripheral_select` output.
 *
 * @param pinmux Pinmux state data.
 * @param output Padring MIO output.
 * @param select Peripheral output select.
 * @return `dif_pinmux_result_t`.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_output_select(
    const dif_pinmux_t *pinmux, dif_pinmux_padring_mio_t output,
    dif_pinmux_peripheral_outsel_t select);

/**
 * Locks out Pin Multiplexer output select for a given padring MIO output.
 *
 * This function is reentrant: calling it while functionality is locked will
 * have no effect and return `kDifPinmuxOk`.
 *
 * @param pinmux Pinmux state data.
 * @param output Padring MIO output.
 * @return `dif_pinmux_result_t`.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_output_select_lock(
    dif_pinmux_t *pinmux, dif_pinmux_padring_mio_t output);

/**
 * Checks whether this Pin Multiplexer input select is locked.
 *
 * @param pinmux A Pin Multiplexer handle.
 * @param output Padring MIO output.
 * @param[out] is_locked Out-param for the locked state.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_output_select_is_locked(
    const dif_pinmux_t *pinmux, dif_pinmux_padring_mio_t output,
    bool *is_locked);

/**
 * TODO
 *
 * @param pinmux Pinmux state data.
 * @param output Padring MIO output.
 * @param config MIO sleep configuration.
 * @return `dif_pinmux_result_t`.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_mio_sleep_configure(
    dif_pinmux_t *pinmux, dif_pinmux_padring_mio_t mio,
    dif_pinmux_sleep_config_t config);

/**
 * Locks out Pin Multiplexer MIO sleep configuration.
 *
 * This function is reentrant: calling it while functionality is locked will
 * have no effect and return `kDifPinmuxOk`.
 *
 * @param pinmux Pinmux state data.
 * @param mio Padring MIO pin.
 * @return `dif_pinmux_result_t`.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_mio_sleep_lock(dif_pinmux_t *pinmux,
                                              dif_pinmux_padring_mio_t mio);

/**
 * Checks whether this Pin Multiplexer MIO sleep configuration is locked.
 *
 * @param pinmux A Pin Multiplexer handle.
 * @param mio Padring MIO pin.
 * @param[out] is_locked Out-param for the locked state.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_mio_sleep_is_locked(const dif_pinmux_t *pinmux,
                                                   dif_pinmux_padring_mio_t mio,
                                                   bool *is_locked);

/**
 * Clears deep sleep behaviour for the Padring MIO pin.
 *
 * When deep sleep mode is enabled for the pin, and the device has entered deep
 * sleep mode; upon wake-up, the deep sleep for the pin can be only cleared
 * through this function. Re-configuring the corresponding pin does not change
 * the state of the pin.
 *
 * @param pinmux A Pin Multiplexer handle.
 * @param mio Padring MIO pin.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_mio_sleep_clear_state(
    const dif_pinmux_t *pinmux, dif_pinmux_padring_mio_t mio);

/**
 * Returns whether a particular Padring MIO pin is deep sleep mode.
 *
 * @param pinmux A Pin Multiplexer handle.
 * @param mio Padring MIO pin.
 * @param[out] in_sleep_mode The pin is in sleep mode.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_mio_sleep_get_state(const dif_pinmux_t *pinmux,
                                                   dif_pinmux_padring_mio_t mio,
                                                   bool *in_sleep_mode);

/**
 * TODO
 *
 * @param pinmux Pinmux state data.
 * @param output Padring DIO output.
 * @param config DIO sleep configuration.
 * @return `dif_pinmux_result_t`.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_dio_sleep_configure(
    dif_pinmux_t *pinmux, dif_pinmux_padring_dio_t dio,
    dif_pinmux_sleep_config_t config);

/**
 * Locks out Pin Multiplexer DIO sleep configuration.
 *
 * This function is reentrant: calling it while functionality is locked will
 * have no effect and return `kDifPinmuxOk`.
 *
 * @param pinmux Pinmux state data.
 * @param dio Padring DIO pin.
 * @return `dif_pinmux_result_t`.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_dio_sleep_lock(dif_pinmux_t *pinmux,
                                              dif_pinmux_padring_dio_t dio);

/**
 * Checks whether this Pin Multiplexer DIO sleep configuration is locked.
 *
 * @param pinmux A Pin Multiplexer handle.
 * @param dio Padring DIO pin.
 * @param[out] is_locked Out-param for the locked state.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_dio_sleep_is_locked(const dif_pinmux_t *pinmux,
                                                   dif_pinmux_padring_dio_t dio,
                                                   bool *is_locked);

/**
 * Clears deep sleep behaviour for the Padring DIO pin.
 *
 * When deep sleep mode is enabled for the pin, and the device has entered deep
 * sleep mode; upon wake-up, the deep sleep for the pin can be only cleared
 * through this function. Re-configuring the corresponding pin does not change
 * the state of the pin.
 *
 * @param pinmux A Pin Multiplexer handle.
 * @param dio Padring DIO pin.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_dio_sleep_clear_state(
    const dif_pinmux_t *pinmux, dif_pinmux_padring_dio_t dio);

/**
 * Returns whether a particular Padring DIO pin is deep sleep mode.
 *
 * @param pinmux A Pin Multiplexer handle.
 * @param dio Padring DIO pin.
 * @param[out] in_sleep_mode The pin is in sleep mode.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_dio_sleep_get_state(const dif_pinmux_t *pinmux,
                                                   dif_pinmux_padring_dio_t dio,
                                                   bool *in_sleep_mode);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // OPENTITAN_SW_DEVICE_LIB_DIF_DIF_PINMUX_H_
