//
//  section_versions.c
//  MegaOTA
//
//  Created by Michael Kwasnicki on 17.12.19.
//  Copyright © 2019 Michael Kwasnicki. All rights reserved.
//

#include <stdint.h>


// section overflow guard
const uint16_t __attribute__((section(".boot_version")))       boot_version      = 0x12;
const uint16_t __attribute__((section(".boot_ota_version")))   boot_ota_version  = 0x34;
const uint16_t __attribute__((section(".main_version")))       main_version      = 0x56;
const uint16_t __attribute__((section(".main_ota_version")))   main_ota_version  = 0x78;
const uint16_t __attribute__((section(".write_version")))      write_version     = 0x90;
const uint16_t __attribute__((section(".write_ota_version")))  write_ota_version = 0xAB;
