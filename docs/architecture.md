# System Architecture

This project has two independent subsystems. They are **not currently networked
together** — there is no real-time data link between the Arduino and the
Raspberry Pi in this codebase. Each runs and is documented on its own.

## 1. Arduino Subsystem — Monitoring & Irrigation

```
 Soil Moisture ─┐
   pH Sensor ────┼──► Arduino Uno ──► LCD (status display)
 Float Switch ───┘         │
                            ├──► Relay ──► Water Pump
                            ├──► Buzzer (low-water alert)
                            ├──► LED (water status indicator)
                            └──► Bluetooth (HC-05/06) ──► phone/serial terminal
```

**Control loop, once per ~500 ms:**
1. Read float switch → determine water availability.
2. Read soil moisture (analog) → convert to a 0–100 scale.
3. Read pH sensor (analog) → convert to voltage → apply linear calibration.
4. **Safety check first:** if water is unavailable and the pump is currently
   on, force it off immediately — this overrides both automatic and manual
   modes.
5. Drive the buzzer and LED from water availability.
6. Push a status line to both `Serial` and `BTSerial` (moisture / pH / water
   status), and update the LCD.
7. Check for an incoming Bluetooth command (`1` = pump on, `0` = pump off,
   `A` = return to automatic mode).
8. If in automatic mode, turn the pump on when moisture is below the
   configured threshold **and** water is available; otherwise turn it off.

## 2. Raspberry Pi Subsystem — Disease Detection

```
 Stored test image (.jpg)
        │
        ▼
  OpenCV preprocessing
  (RGB convert → resize 224x224 → normalize)
        │
        ▼
  TensorFlow Lite interpreter
  (plant_disease_model.tflite)
        │
        ▼
  argmax over 38-class output
        │
        ▼
  Printed result: healthy / disease name
```

This is a single-shot script: it runs once on one image file per invocation.
It does not read from a camera and does not run continuously.

## What this project does *not* currently include

To keep documentation and any future collaborators' expectations accurate:

- No TDS sensor.
- No live camera feed for disease detection.
- No cloud connectivity or remote dashboard.
- No mobile app (control is via Bluetooth serial commands only).
- No real-time communication between the Arduino and Raspberry Pi.
- No included circuit diagram, PCB layout, 3D model, dataset, or trained
  `.tflite` model file in this repository (see [Limitations](../README.md#limitations--known-issues)).
