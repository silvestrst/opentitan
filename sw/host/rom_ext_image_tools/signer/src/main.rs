// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#![deny(warnings)]
#![deny(unused)]
#![deny(unsafe_code)]

use std::env;
use std::fs;
use std::path::Path;

use rom_ext_config::parser::ParsedConfig;
use rom_ext_image::image::RawImage;

use mundane::hash::Sha256;
use mundane::public::DerPrivateKey;
use mundane::public::Signature;
use mundane::public::rsa::B3072;
use mundane::public::rsa::RsaPkcs1v15;
use mundane::public::rsa::RsaSignature;
use mundane::public::rsa::RsaPrivKey;

fn main() {
    let arg: String = env::args().nth(1).expect("Config path is missing");

    let config_path = Path::new(&arg);

    // Parse the config.
    let config = ParsedConfig::new(&config_path);

    // Read raw binary.
    let image_path = Path::new(&config.input_files.image_path);
    let mut image = RawImage::new(&image_path);

    // Get private key.
    let private_key_path = Path::new(&config.input_files.private_key_der_path);
    let private_key_der = fs::read(private_key_path)
        .expect("Failed to read the image!");

    // Update manifest fields
    image.update_static_fields(&config);

    let exponent = &signature_key_public_exponent_le();
    image.update_exponent_field(exponent);

    let modulus = &signature_key_modulus_le();
    image.update_modulus_field(modulus);

    // Convert ASN.1 DER private key into Mundane RsaPrivKey.
    let private_key = RsaPrivKey::parse_from_der(&private_key_der)
        .expect("Failed to parse private key!");

    // Sign the image.
    let image_sign_data = image.data_to_sign();
    let device_usage_value =
        &device_usage_value(&config.input_files.usage_constraints_path);
    let system_state = &system_state_value();

    let mut message_to_sign = Vec::<u8>::new();
    message_to_sign.extend_from_slice(system_state);
    message_to_sign.extend_from_slice(device_usage_value);
    message_to_sign.extend_from_slice(image_sign_data);

    let signature = RsaSignature::<B3072, RsaPkcs1v15, Sha256>::
    sign(&private_key, &message_to_sign).expect("Failed to sign!");

    // Update signature field.
    image.update_signature_field(signature.bytes());

    // Write image to disk.
    image.write_file();
}

/// Generate a dummy signature key public exponent.
///
/// Eventually this value will be obtained from the private key.
fn signature_key_public_exponent_le() -> Vec<u8> {
    let dummy: Vec<u8> = vec![0xA5; 1];

    dummy
}

/// Generate a dummy signature key modulus.
///
/// Eventually this value will be obtained from the private key.
fn signature_key_modulus_le() -> Vec<u8> {
    let dummy: Vec<u8> = vec![0xA5; 384];

    dummy
}

/// Generates the device usage value.
///
/// This value is extrapolated from the ROM_EXT manifest usage_constraints
/// field, and does not reside in the ROM_EXT manifest directly.
pub fn device_usage_value(dir: &str) -> Vec<u8>{
    let usage_constraints_path = Path::new(dir);
    let _usage_constraints = fs::read(usage_constraints_path)
        .expect("Failed to read usage constraints!");

    // TODO: generate the device_usage_value from usage_constraints.
    //       meanwhile use a hard-coded vector.

    let dummy: Vec<u8> = vec![0xA5; 1024];

    dummy
}

/// Generates the system state value.
///
/// TODO: it is not clear at the moment what the format and size of this
///       value is. For the time being assume u32.
pub fn system_state_value() -> [u8; 4] {
    let dummy: u32 = 0xA5A5A5A5;

    dummy.to_le_bytes()
}
