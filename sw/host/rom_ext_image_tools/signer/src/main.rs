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

//use ring::signature::KeyPair;
//use ring::signature::RsaKeyPair;

fn main() {
    let arg: String = env::args().nth(1).expect("Config path is missing");

    let config_path = Path::new(&arg);

    // Parse the config.
    let config = ParsedConfig::new(&config_path);

    // Read raw binary.
    let image_path = Path::new(&config.input_files.image_path);
    let mut raw_image = RawImage::new(&image_path);

    // Get private key.
    let private_key_path = Path::new(&config.input_files.private_key_der_path);
    let private_key_der = fs::read(private_key_path)
        .expect("Failed to read the image!");

    // Update fields.
    raw_image.update_generic_fields(&config);
//    update_public_key_fields(&mut raw_image, &private_key_der);

    // Sign the image.
    const MESSAGE: &[u8] = b"Hello World!";

    let private_key = RsaPrivKey::parse_from_der(&private_key_der)
        .expect("Failed to parse private key!");
    let signature = RsaSignature::<B3072, RsaPkcs1v15, Sha256>::
    sign(&private_key, &MESSAGE).expect("Failed to sign!");

    // Update signature field.
    raw_image.update_signature_field(signature.bytes());

    // Write image to disk.
    raw_image.write_file();
}

//fn update_public_key_fields(raw_image: &mut RawImage, private_key_der: &[u8]) {
//    let ring_key_pair = RsaKeyPair::from_der(private_key_der)
//        .expect("Failed to create Ring RsaKeyPair from der!");
//
//    let mut modulus = ring_key_pair
//        .public_key()
//        .modulus()
//        .big_endian_without_leading_zero()
//        .to_vec();
//
//    let mut exponent = ring_key_pair
//        .public_key()
//        .exponent()
//        .big_endian_without_leading_zero()
//        .to_vec();
//
//    // Make little-endian.
//    modulus.reverse();
//    exponent.reverse();
//
//    raw_image.update_exponent_field(&exponent);
//    raw_image.update_modulus_field(&modulus);
//}
