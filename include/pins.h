#ifndef PINS_H
#define PINS_H

// ==========================================
// YD-ESP32-S3 (N16R8) SAFE PIN DEFINITIONS
// ==========================================

// Hardware SPI Bus (RC522 RFID, SD Card, Displays)
#define PIN_SPI_SCK   12
#define PIN_SPI_MISO  13
#define PIN_SPI_MOSI  11
#define PIN_SPI_SS    10
#define PIN_RFID_RST   9

// Safe General Purpose I/O
#define PIN_LED        4
#define PIN_BUTTON     0  // Onboard BOOT button

// LOCKED PINS (DO NOT TOUCH):
// - GPIO 33-37: 8MB Octal PSRAM
// - GPIO 19-20: Native USB D-/D+
// - GPIO 43-44: UART COM Port
// - GPIO 45-46: Boot Strapping

#endif
