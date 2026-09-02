# Arduino Pin Connections

| Component                         | Arduino Pin  | Notes                              |
| ---------------------------------- | ------------ | ----------------------------------- |
| Soil Moisture Sensor               | A0           | Analog input                       |
| pH Sensor                          | A1           | Analog input                       |
| Float Switch / Water Level Sensor  | D7           | `INPUT_PULLUP`, LOW = water present |
| Pump Relay                         | D8           | Active-low (LOW = pump ON)         |
| Buzzer                             | D9           | Tone alert when water unavailable  |
| Bluetooth Module RX                | D10          | `SoftwareSerial` RX on Arduino     |
| Bluetooth Module TX                | D11          | `SoftwareSerial` TX on Arduino     |
| LED (water status indicator)       | D13          | ON when water available            |
| 16x2 I2C LCD                       | SDA / SCL    | I2C address `0x27`                 |

**Relay logic:** the relay module used is active-low — `digitalWrite(PUMP_PIN, HIGH)` turns the pump OFF, `LOW` turns it ON. This is opposite to many beginner relay tutorials that assume active-high, so double-check your specific relay board's datasheet before wiring.

**Float switch logic:** wired with `INPUT_PULLUP`. A LOW reading is treated as "water available" (switch closed), HIGH as "no water" (switch open/circuit broken).
