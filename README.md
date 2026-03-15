# 🐱 myaotron

Cat-on-counter detection and deterrent system powered by [HUSKYLENS 2](https://wiki.dfrobot.com/sen0638/docs/22636/) AI camera.

Detects when your cat is on the kitchen counter and gives them a gentle puff of air (or water mist) to discourage the behavior — automatically and humanely.

## How It Works

The HUSKYLENS 2 uses its built-in Object Recognition (MS COCO 80 classes) to detect both **cats** and **surfaces** (dining table/counter) in the camera frame. The system supports two detection modes:

| Mode | Strategy | Best For |
|------|----------|----------|
| **Mode 3** (default) | Dual-object overlap — triggers only when a cat's bounding box overlaps with a detected counter surface | General use, fewer false positives |
| **Mode 1** (fallback) | Camera-only — triggers on any cat detection in frame | Camera aimed exclusively at the counter |

When a cat-on-counter event is detected, the system activates a relay/MOSFET that controls a solenoid valve (water mist) or air pump (puff of air).

## Parts List

| Part | Description |
|------|-------------|
| [HUSKYLENS 2 (SEN0638)](https://www.dfrobot.com/product-2828.html) | AI camera with built-in object recognition |
| [DFRduino UNO R3](https://www.dfrobot.com/product-838.html) | Arduino-compatible main board (or ESP32) |
| Gravity IO Expansion Shield V7.1 | Convenient pin breakout for Arduino |
| Relay module or MOSFET | To switch the deterrent on/off |
| Solenoid valve or mini air pump | The actual deterrent mechanism |
| 12V power supply | For the solenoid/pump (adjust to your component) |
| USB-C cable | Additional power for HUSKYLENS 2 |
| 4-pin I2C cable | HUSKYLENS 2 ↔ Arduino connection |

## Wiring

```
HUSKYLENS 2          Arduino Uno
─────────────        ───────────
SDA  ──────────────► A4 (SDA)
SCL  ──────────────► A5 (SCL)
GND  ──────────────► GND
(Power via USB-C to separate source)

Relay Module         Arduino Uno
────────────         ───────────
Signal ────────────► D7 (DETERRENT_PIN)
VCC    ────────────► 5V
GND    ────────────► GND

Relay NO/COM ──────► Solenoid valve / air pump circuit
```

> **Note:** The Arduino Uno's I2C pins cannot supply enough power for HUSKYLENS 2. Connect a separate USB-C cable from HUSKYLENS 2 to a power source.

## Setup

### 1. Install the Library

Download the [DFRobot_HuskylensV2 Arduino library](https://github.com/DFRobot/DFRobot_HuskylensV2) and install it:

- Download the ZIP from GitHub
- In Arduino IDE: **Sketch → Include Library → Add .ZIP Library**
- Select the downloaded ZIP file

### 2. Train HUSKYLENS 2

Before uploading the sketch, teach HUSKYLENS 2 to recognize your cat and counter:

1. Power on HUSKYLENS 2 and select **Object Recognition**
2. Point the camera at your **cat** and press **Button-A** to learn it as **ID 1**
3. Point the camera at your **counter/table surface** and press **Button-A** to learn it as **ID 2**
4. (Optional) Use **Export Model** in settings to save your trained model

### 3. Configure

Edit `myaotron/config.h` to adjust:

- **`DETECTION_MODE`** — `3` for dual-object overlap (default), `1` for camera-only
- **`DETERRENT_PIN`** — digital pin connected to your relay (default: `7`)
- **`SPRAY_DURATION_MS`** — how long each spray lasts (default: `500`ms)
- **`SPRAY_COOLDOWN_MS`** — minimum time between sprays (default: `5000`ms)
- **`MIN_CONFIDENCE`** — detection confidence threshold (default: `30`)
- **`OVERLAP_THRESHOLD`** — how much the cat must overlap the surface (default: `0.3`)

### 4. Upload

1. Open `myaotron/myaotron.ino` in Arduino IDE
2. Select your board (Arduino Uno, ESP32, etc.)
3. Upload the sketch
4. Open Serial Monitor (9600 baud) to see detection output

## Detection Logic (Mode 3)

```
┌─────────────────────────────────────┐
│         HUSKYLENS 2 Frame           │
│                                     │
│    ┌──────────┐                     │
│    │   cat    │ ← detected cat      │
│    │  (ID 1)  │   bounding box      │
│    └────┬─────┘                     │
│         │ overlap?                  │
│  ┌──────┴──────────────────────┐    │
│  │     dining table (ID 2)     │    │
│  │     (counter surface)       │    │
│  └─────────────────────────────┘    │
│                                     │
└─────────────────────────────────────┘

If cat bbox overlaps surface bbox:
  → horizontal overlap ≥ 30%
  → cat's bottom edge within surface bounds
  → SPRAY!
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "HUSKYLENS 2 not found" | Check I2C wiring (SDA→A4, SCL→A5). Ensure separate USB-C power to HUSKYLENS 2 |
| Cat detected but no spray | Check relay wiring on pin D7. Open Serial Monitor to see detection output |
| Too many false positives | Increase `MIN_CONFIDENCE` or `OVERLAP_THRESHOLD` in config.h |
| Sprays when cat is on floor | Make sure counter is learned as ID 2. Try repositioning camera angle |
| Counter not detected | Lower the Detection Threshold in HUSKYLENS 2 settings. Re-learn the surface |

## License

MIT
