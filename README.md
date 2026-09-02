# Smart Plant Monitoring & Automated Irrigation System

A two-part plant care system combining an **Arduino Uno** for real-time soil/water
monitoring and automated irrigation, with a **Raspberry Pi** running a
TensorFlow Lite model for plant disease classification from a leaf image.

The two subsystems are independent — they do not currently share a live data
link. See [Architecture](docs/architecture.md) for how each one works.

## Features

**Arduino — Monitoring & Irrigation**
- Reads soil moisture, pH, and water level (float switch) continuously.
- Displays live readings and pump status on a 16x2 I2C LCD.
- Automatic irrigation: pump turns on when soil moisture drops below a
  configurable threshold, provided water is available.
- Manual override via Bluetooth (`1` = pump on, `0` = pump off, `A` = back to
  automatic mode).
- Safety interlock: the pump is forced off whenever the water source is
  empty/low, even if it was switched on manually — this cannot be overridden.
- Audible (buzzer) and visual (LED) low-water alerts.

**Raspberry Pi — Plant Disease Detection**
- Classifies a plant leaf image into one of 38 classes (healthy or a specific
  disease) using a TensorFlow Lite model.
- Preprocessing: RGB conversion → resize to 224x224 → normalization.
- Runs on a single stored image file per execution (not a live camera feed).

## Repository Structure

```
smart-plant-monitoring-system/
├── arduino/
│   └── plant_monitoring_irrigation/
│       └── plant_monitoring_irrigation.ino
├── raspberry-pi/
│   ├── disease_detection.py
│   └── requirements.txt
├── docs/
│   ├── architecture.md
│   ├── pin_connections.md
│   └── hardware_components.md
├── LICENSE
├── .gitignore
└── README.md
```

## Hardware & Software

Full component and dependency lists: [docs/hardware_components.md](docs/hardware_components.md)

Pin connection table: [docs/pin_connections.md](docs/pin_connections.md)

## Getting Started

### Arduino

1. Install the Arduino IDE.
2. Install the `LiquidCrystal_I2C` library (Library Manager or from
   [github.com/johnrickman/LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)).
3. Wire the components according to [docs/pin_connections.md](docs/pin_connections.md).
4. Open `arduino/plant_monitoring_irrigation/plant_monitoring_irrigation.ino`
   in the Arduino IDE, select your board/port, and upload.
5. Open the Serial Monitor (9600 baud) to view live sensor output, or pair a
   Bluetooth terminal app with the HC-05/06 module to view status and send
   pump commands.

### Raspberry Pi

1. Install dependencies:
   ```bash
   pip install -r raspberry-pi/requirements.txt
   ```
   If `tflite-runtime` isn't available for your Pi's OS/architecture, install
   full `tensorflow` instead and change the import line in
   `disease_detection.py` from `import tflite_runtime.interpreter as tflite`
   to `import tensorflow.lite as tflite`.
2. Provide your own trained `.tflite` model and a test image (neither is
   included in this repository — see [Limitations](#limitations--known-issues)).
3. Edit `MODEL_PATH` and `IMAGE_PATH` at the top of `disease_detection.py` to
   point to your files.
4. Run:
   ```bash
   python3 raspberry-pi/disease_detection.py
   ```

## Limitations & Known Issues

This repository is documented strictly to what's actually implemented —
please read this section before assuming functionality that isn't here.

- **No trained model file included.** `disease_detection.py` requires a
  `.tflite` model that is not part of this repo. The script will not run
  out of the box until you supply one trained on a compatible 38-class label
  set.
- **No live camera support.** Disease detection currently runs on a single
  static image file, not a camera feed.
- **No TDS sensor.**
- **No cloud connectivity, web dashboard, or mobile app** — control is
  limited to Bluetooth serial commands.
- **No real-time link between the Arduino and Raspberry Pi** — they operate
  as separate subsystems.
- **Minimal error handling** in `disease_detection.py` — an unreadable image
  path will raise a clear error, but the model-loading step does not
  currently handle a missing/corrupt `.tflite` file gracefully.
- **Sensor calibration is hardware-specific.** The soil moisture mapping
  (`map(rawMoisture, 550, 0, 0, 100)`) and pH formula
  (`3.5 * voltage + phOffset`) were tuned for the original sensors used in
  this build. Re-calibrate both against your own sensors before trusting the
  readings.
- No circuit diagram, PCB files, 3D models, dataset, or project photos are
  included in this repository at this time.

## Possible Future Improvements

*(Not implemented — listed as ideas only, so they aren't mistaken for
current features.)*

- Add a live camera pipeline for the Raspberry Pi disease detection instead
  of a static test image.
- Add a serial/Bluetooth link between the Arduino and Raspberry Pi so
  disease-detection results can influence irrigation logic.
- Add a TDS (total dissolved solids) sensor for water quality monitoring.
- Add persistent logging of sensor readings (SD card or Pi-side database).
- Add a simple mobile or web UI in place of raw Bluetooth serial commands.
- Add automated calibration routines for the soil moisture and pH sensors.

## License

Distributed under the [MIT License](LICENSE).
