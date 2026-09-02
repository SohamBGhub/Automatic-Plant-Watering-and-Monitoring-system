# Components & Dependencies

## Arduino Subsystem — Hardware

- Arduino Uno
- Soil moisture sensor
- pH sensor
- Float switch / water level sensor
- 16x2 I2C LCD display
- Bluetooth module (HC-05, HC-06, or equivalent)
- Relay module (active-low)
- Water pump
- Buzzer
- LED

## Arduino Subsystem — Libraries

- [`Wire`](https://www.arduino.cc/reference/en/language/functions/communication/wire/) (built-in)
- [`LiquidCrystal_I2C`](https://github.com/johnrickman/LiquidCrystal_I2C)
- [`SoftwareSerial`](https://www.arduino.cc/en/Reference/SoftwareSerial) (built-in)

## Raspberry Pi Subsystem — Hardware

- Raspberry Pi (any model capable of running Python 3 and TensorFlow Lite Runtime)

## Raspberry Pi Subsystem — Software

- Python 3
- `tflite-runtime`
- `opencv-python`
- `numpy`
- A trained `.tflite` plant disease classification model (38 classes,
  PlantVillage-style — **not included in this repository**, see
  [Limitations](../README.md#limitations--known-issues))
