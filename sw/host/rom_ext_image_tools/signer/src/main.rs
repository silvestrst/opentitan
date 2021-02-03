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

    image.update_static_fields(&config);

    // Convert ASN.1 DER private key into Mundane RsaPrivKey.
    let private_key = RsaPrivKey::parse_from_der(&private_key_der)
        .expect("Failed to parse private key!");

    // Sign the image.
    let data_to_sign = image.data_to_sign();
    let signature = RsaSignature::<B3072, RsaPkcs1v15, Sha256>::
    sign(&private_key, data_to_sign).expect("Failed to sign!");

    // Update signature field.
    image.update_signature_field(signature.bytes());

    // Write image to disk.
    image.write_file();
}
