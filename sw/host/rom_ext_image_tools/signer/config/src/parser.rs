// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

use std::fs;
use std::path::Path;

use serde_derive::Deserialize;
use serde_hjson::Value;

/// Configuration used for the manifest initialisation.
#[derive(Deserialize, Debug)]
pub struct ParsedConfig {
    pub input_files: InputFiles,
    pub peripheral_lockdown_info: PeripheralLockdownInfo,
    pub manifest_identifier: String,
    pub image_version: String,
    pub extension0_offset: String,
    pub extension0_checksum: String,
    pub extension1_offset: String,
    pub extension1_checksum: String,
    pub extension2_offset: String,
    pub extension2_checksum: String,
    pub extension3_offset: String,
    pub extension3_checksum: String,
}

/// Input files that are required for signing.
#[derive(Deserialize, Debug)]
pub struct InputFiles {
    pub image_path: String,
    pub private_key_der_path: String,
    pub usage_constraints_path: String,
    pub system_state_value_path: String,
}

/// Peripheral Lockdown Information configuration data.
///
/// This data is used to produce 128-bit encoded manifest field.
#[derive(Deserialize, Debug)]
pub struct PeripheralLockdownInfo {
    pub value: u32,
}

impl ParsedConfig {
    pub fn new(config: &Path) -> Self {
        // Read the entire configuration file.
        let config_data = fs::read_to_string(config).expect("Failed to read the config file!");

        let data: Value =
            serde_hjson::from_str(&config_data).expect("Failed to parse the hjson config file!");

        let deserialized: ParsedConfig =
            serde_hjson::from_value(data).expect("Failed to deserialize hjson config data!");

        deserialized
    }
}
