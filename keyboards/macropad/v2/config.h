// Copyright 2022 ziptyze (@ziptyze)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define ENCODER_MAP_KEY_DELAY 10

// OLED is on the I2C0 peripheral; QMK's driver names the pins I2C1_* whichever peripheral is used.
#define I2C_DRIVER I2CD0
#define I2C1_SDA_PIN GP20
#define I2C1_SCL_PIN GP21

#define WS2812_PIO_USE_PIO1

#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET // Activates the double-tap behavior
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 1000U // Timeout window in ms in which the double tap can occur.
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED 25 // Specify a optional status led by GPIO number which blinks when entering the bootloader
