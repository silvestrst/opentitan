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
    let mut raw_image = RawImage::new(&image_path);

    // Sign
    let private_key_path = Path::new(&config.input_files.private_key_der_path);
    let private_key_der = fs::read(private_key_path)
        .expect("Failed to read the image!");
    const MESSAGE: &[u8] = b"Hello World!";
    let private_key = RsaPrivKey::parse_from_der(&private_key_der)
        .expect("Failed to parse private key!");
    let signature = RsaSignature::<B3072, RsaPkcs1v15, Sha256>::
    sign(&private_key, &MESSAGE).expect("Failed to sign!");
    fs::write("./mundane_signature.bin", signature.bytes())
        .expect("Failed to write the signature!");

    // Modify raw binary.
    raw_image.update_generic_fields(&config);
    raw_image.write_file();
}
