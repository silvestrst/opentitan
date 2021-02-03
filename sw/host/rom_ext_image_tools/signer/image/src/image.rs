// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

use std;
use std::ffi::OsString;
use std::fs;
use std::path::Path;
use std::path::PathBuf;

use crate::manifest;
use rom_ext_config::parser::ParsedConfig;
use rom_ext_config::parser::PeripheralLockdownInfo;

/// Stripped binary image buffer.
pub struct RawImage {
    data: Vec<u8>,
    path: PathBuf,
}

/// Buffer manipulation API.
impl RawImage {
    /// Creates the new image buffer.
    ///
    /// The data is read from the requested raw binary file.
    pub fn new(image_path: &Path) -> Self {
        let data = fs::read(image_path).expect("Failed to read the image!");

        RawImage {
            data: data,
            path: image_path.to_path_buf(),
        }
    }

    /// Updates the fields from the configuration file.
    ///
    /// This function updates the image manifest data with values parsed from
    /// the configuration file (known ahead of time). Some of the other fields
    /// like signature key public exponent and modulus, are obtained at
    /// run-time.
    pub fn update_static_fields(&mut self, config: &ParsedConfig) {
        // TODO checks to make sure that the config values (strings) are not
        // bigger than the actual field size.

        let mut update = |value, offset| {
            let bytes = str_to_vec_u8(value);
            self.update_field(&bytes, offset);
        };

        update(
            &config.manifest_identifier,
            manifest::ROM_EXT_MANIFEST_IDENTIFIER_OFFSET,
        );
        update(&config.image_version, manifest::ROM_EXT_IMAGE_VERSION_OFFSET);
        update(
            &config.image_timestamp,
            manifest::ROM_EXT_IMAGE_TIMESTAMP_OFFSET,
        );
        update(
            &config.extension0_checksum,
            manifest::ROM_EXT_EXTENSION0_CHECKSUM_OFFSET,
        );
        update(
            &config.extension1_checksum,
            manifest::ROM_EXT_EXTENSION1_CHECKSUM_OFFSET,
        );
        update(
            &config.extension2_checksum,
            manifest::ROM_EXT_EXTENSION2_CHECKSUM_OFFSET,
        );
        update(
            &config.extension3_checksum,
            manifest::ROM_EXT_EXTENSION3_CHECKSUM_OFFSET,
        );

        self.update_usage_constraints_field(
            &config.input_files.usage_constraints_path);

        self.update_peripheral_lockdown_info_field(
            &config.peripheral_lockdown_info);
    }

    /// TODO
    pub fn update_exponent_field(&mut self, exponent: &[u8]) {
        let offset = manifest::ROM_EXT_SIGNATURE_KEY_PUBLIC_EXPONENT_OFFSET;
        self.update_field(exponent, offset);
    }

    /// TODO
    pub fn update_modulus_field(&mut self, modulus: &[u8]) {
        let offset = manifest::ROM_EXT_SIGNATURE_KEY_MODULUS_OFFSET;
        self.update_field(modulus, offset);
    }

    /// TODO
    pub fn update_signature_field(&mut self, signature: &[u8]) {
        self.update_field(signature, manifest::ROM_EXT_IMAGE_SIGNATURE_OFFSET);
    }

    /// TODO
    pub fn data_to_sign(&self) -> &[u8] {
        let offset = manifest::ROM_EXT_SIGNED_AREA_START_OFFSET as usize;
        &self.data[offset..]
    }

    /// Writes the image buffer contents into a file.
    ///
    /// Places the new file alongside the original, with a "new_" prefix.
    pub fn write_file(&self) {
        let file_name = self.path.file_name().expect("Failed to get file stem!");

        let mut new_file_name = OsString::from("new_");
        new_file_name.push(file_name);

        let output_file = self.path.with_file_name(new_file_name);

        fs::write(output_file, &self.data).expect("Failed to write the new binary file!");
    }

    /// TODO
    fn update_usage_constraints_field(&mut self, dir: &str) {
        // Update fields from config.
        let usage_constraints_path = Path::new(dir);
        let usage_constraints = fs::read(usage_constraints_path)
            .expect("Failed to read usage constraints!");

        let offset = manifest::ROM_EXT_USAGE_CONSTRAINTS_OFFSET;
        self.update_field(&usage_constraints, offset);
    }

    /// TODO
    fn update_peripheral_lockdown_info_field(
        &mut self, info: &PeripheralLockdownInfo) {

        // TODO
    }

    /// TODO
    fn update_field(&mut self, field_data: &[u8], field_offset: u32) {
        let image = &mut self.data;
        let begin = field_offset as usize;
        let end = begin + field_data.len();
        image.splice(begin..end, field_data.iter().cloned());
    }
}

/// Converts hex/decimal uint string into a little endian byte vector.
///
/// Note: only understands unsigned u64 and u32 integers.
fn str_to_vec_u8(s: &str) -> Vec<u8> {
    let value = match s.starts_with("0x") {
        true => u64::from_str_radix(s.trim_start_matches("0x"), 16)
            .expect("Failed to parse string to u64!"),
        false => s.parse::<u64>().expect("Failed to parse string to u64!"),
    };

    if value <= (u32::MAX as u64) {
        (value as u32).to_le_bytes().to_vec()
    } else {
        value.to_le_bytes().to_vec()
    }
}
