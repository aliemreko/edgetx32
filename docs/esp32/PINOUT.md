# ESP32-S3 reference DIY radio pinout

Valid ESP32-S3 GPIOs: `0–21`, `26–48`.  
Octal PSRAM modules (**N16R8**) must avoid **GPIO 33–37**.

## Analog (ADC1)

| Function | GPIO |
|----------|------|
| LH / Rud | 1 |
| LV / Ele | 2 |
| RV / Thr | 4 |
| RH / Ail | 5 |
| Pot S1 | 6 |
| Pot S2 | 7 |
| VBAT sense | 8 (external divider) |

## Modules (UART)

| Function | GPIO |
|----------|------|
| Internal CRSF/ELRS TX/RX | 43 / 44 |
| Internal module PWR | 3 |
| External module TX/RX | 17 / 18 |
| External module PWR | 8* |

\* If VBAT also uses GPIO 8, move EXT PWR to an IO expander or free GPIO.

## Display / storage / audio

| Function | GPIO |
|----------|------|
| LCD SPI MOSI/SCLK/CS/DC/RST | 11 / 12 / 10 / 13 / 9 |
| Backlight PWM | 47 |
| SD SPI MOSI/MISO/SCLK/CS | 38 / 39 / 40 / 41 |
| I2S BCK/WS/DOUT | 26 / 27 / 28 |
| Haptic PWM | 48 |

## Keys / switches / encoder

JSON lists SA–SF plus physical keys for a full radio. On a bare DevKit many
of those GPIOs collide with LCD/SD — use:

- **Software keyboard + touch** for bring-up (`SOFTWARE_KEYBOARD`)
- **PCA9555 / AW9523** on I2C for production switch/key matrix
- Rotary encoder on GPIO 19 / 20 (USB-JTAG pins — disable USB Serial/JTAG if used)

Remap freely in `radio/src/targets/esp32s3/hal.h` and `esp32s3.json`.
