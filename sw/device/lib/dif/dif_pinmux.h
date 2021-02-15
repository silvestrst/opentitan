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

  // Other fields, if necessary.
} dif_pinmux_params_t;

/**
 * Runtime configuration for Pin Multiplexer.
 *
 * This struct describes runtime information for one-time configuration of the
 * hardware.
 */
typedef struct dif_pinmux_config {
  int dummy;
} dif_pinmux_config_t;

/**
 * A handle to Pin Multiplexer.
 *
 * This type should be treated as opaque by users.
 */
typedef struct dif_pinmux {
  dif_pinmux_params_t params;

  // Other fields, if necessary.
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
  // Remove this variant if you don't need it.
  kDifPinmuxLocked = 3,
} dif_pinmux_result_t;


/**
 * Parameters for a Pin Multiplexer transaction.
 */
typedef struct dif_pinmux_transaction {
  int dummy;
} dif_pinmux_transaction_t;

/**
 * An output location for a Pin Multiplexer transaction.
 */
typedef struct dif_pinmux_output {
  int dummy;
} dif_pinmux_output_t;

/**
 * A Pin Multiplexer interrupt request type.
 */
typedef enum dif_pinmux_irq {
  eDifPinmuxDummy = 0,
} dif_pinmux_irq_t;

/**
 * A snapshot of the enablement state of the interrupts for Pin Multiplexer.
 *
 * This is an opaque type, to be used with the `dif_pinmux_irq_disable_all()` and
 * `dif_pinmux_irq_restore_all()` functions.
 */
typedef uint32_t dif_pinmux_irq_snapshot_t;

/**
 * Calculates information needed to safely call a DIF. Functions like this
 * should be used instead of global variables or #defines.
 *
 * This function does not actuate the hardware.
 *
 * @param params Hardware instantiation parameters.
 * @return The information required.
 */
DIF_WARN_UNUSED_RESULT
uint32_t dif_pinmux_get_size(dif_pinmux_params_t params);

/**
 * Creates a new handle for Pin Multiplexer.
 *
 * This function does not actuate the hardware.
 *
 * @param params Hardware instantiation parameters.
 * @param[out] handle Out param for the initialized handle.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_init(dif_pinmux_params_t params,
                                              dif_pinmux_t *handle);

/**
 * Configures Pin Multiplexer with runtime information.
 *
 * This function should need to be called once for the lifetime of `handle`.
 *
 * @param handle A Pin Multiplexer handle.
 * @param config Runtime configuration parameters.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_configure(const dif_pinmux_t *handle,
                                                   dif_pinmux_config_t config);

/**
 * Begins a Pin Multiplexer transaction.
 *
 * Each call to this function should be sequenced with a call to
 * `dif_pinmux_end()`.
 *
 * @param handle A Pin Multiplexer handle.
 * @param transaction Transaction configuration parameters.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_start(const dif_pinmux_t *handle,
                                               dif_pinmux_transaction_t transaction);

/** Ends a Pin Multiplexer transaction, writing the results to the given output..
 *
 * @param handle A Pin Multiplexer handle.
 * @param output Transaction output parameters.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_end(const dif_pinmux_t *handle,
                                             dif_pinmux_output_t output);

/**
 * Locks out Pin Multiplexer functionality.
 *
 * This function is reentrant: calling it while functionality is locked will
 * have no effect and return `kDifPinmuxOk`.
 *
 * @param handle A Pin Multiplexer handle.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_lock(const dif_pinmux_t *handle);

/**
 * Checks whether this Pin Multiplexer is locked.
 *
 * @param handle A Pin Multiplexer handle.
 * @param[out] is_locked Out-param for the locked state.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_is_locked(const dif_pinmux_t *handle,
                                                   bool *is_locked);

/**
 * Returns whether a particular interrupt is currently pending.
 *
 * @param handle A Pin Multiplexer handle.
 * @param irq An interrupt type.
 * @param[out] is_pending Out-param for whether the interrupt is pending.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_irq_is_pending(const dif_pinmux_t *handle,
                                                        dif_pinmux_irq_t irq,
                                                        bool *is_pending);

/**
 * Acknowledges a particular interrupt, indicating to the hardware that it has
 * been successfully serviced.
 *
 * @param handle A Pin Multiplexer handle.
 * @param irq An interrupt type.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_irq_acknowledge(const dif_pinmux_t *handle,
                                                         dif_pinmux_irq_t irq);

/**
 * Checks whether a particular interrupt is currently enabled or disabled.
 *
 * @param handle A Pin Multiplexer handle.
 * @param irq An interrupt type.
 * @param[out] state Out-param toggle state of the interrupt.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_irq_get_enabled(const dif_pinmux_t *handle,
                                                         dif_pinmux_irq_t irq,
                                                         dif_pinmux_toggle_t *state);

/**
 * Sets whether a particular interrupt is currently enabled or disabled.
 *
 * @param handle A Pin Multiplexer handle.
 * @param irq An interrupt type.
 * @param state The new toggle state for the interrupt.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_irq_set_enabled(const dif_pinmux_t *handle,
                                                         dif_pinmux_irq_t irq,
                                                         dif_pinmux_toggle_t state);

/**
 * Forces a particular interrupt, causing it to be serviced as if hardware had
 * asserted it.
 *
 * @param handle A Pin Multiplexer handle.
 * @param irq An interrupt type.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_irq_force(const dif_pinmux_t *handle,
                                                   dif_pinmux_irq_t irq);

/**
 * Disables all interrupts, optionally snapshotting all toggle state for later
 * restoration.
 *
 * @param handle A Pin Multiplexer handle.
 * @param[out] snapshot Out-param for the snapshot; may be `NULL`.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_irq_disable_all(const dif_pinmux_t *handle,
                                                         dif_pinmux_irq_snapshot_t *snapshot);

/**
 * Restores interrupts from the given snapshot.
 *
 * This function can be used with `dif_pinmux_irq_disable_all()` to temporary
 * interrupt save-and-restore.
 *
 * @param handle A Pin Multiplexer handle.
 * @param snapshot A snapshot to restore from.
 * @return The result of the operation.
 */
DIF_WARN_UNUSED_RESULT
dif_pinmux_result_t dif_pinmux_irq_restore_all(const dif_pinmux_t *handle,
                                                         const dif_pinmux_irq_snapshot_t *snapshot);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // OPENTITAN_SW_DEVICE_LIB_DIF_DIF_PINMUX_H_
