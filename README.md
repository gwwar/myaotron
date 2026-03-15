# 🐱 myaotron

Cat-on-counter detection and deterrent system powered by [HUSKYLENS 2](https://wiki.dfrobot.com/sen0638/docs/22636/) AI camera.

Detects when your cat is on the kitchen counter and gives them a gentle puff of air (or water mist) to discourage the behavior — automatically and humanely.

## How It Works

The HUSKYLENS 2 uses its built-in Object Recognition (MS COCO 80 classes) to detect both **cats** and **surfaces** (dining table/counter) in the camera frame. The system supports two detection modes:

| Mode | Strategy | Best For |
|------|----------|----------|
| **MODE_DUAL_OVERLAP** (default) | Dual-object overlap — triggers only when a cat's bounding box overlaps with a detected counter surface | General use, fewer false positives |
| **MODE_CAMERA_ONLY** (fallback) | Camera-only — triggers on any cat detection in frame | Camera aimed exclusively at the counter |

When a cat-on-counter event is detected, the system activates a solenoid valve that releases a puff of air from a pressurized reservoir. An optional 12V diaphragm pump keeps the reservoir charged automatically between sprays.

## Parts List

| Part | Description |
|------|-------------|
| [HUSKYLENS 2 (SEN0638)](https://www.dfrobot.com/product-2828.html) | AI camera with built-in object recognition |
| [DFRduino UNO R3](https://www.dfrobot.com/product-838.html) | Arduino-compatible main board (or ESP32) |
| Gravity IO Expansion Shield V7.1 | Convenient pin breakout for Arduino |
| 2-channel relay module | Controls solenoid valve + air pump |
| 12V normally-closed solenoid valve | Releases air from reservoir on detection |
| 12V diaphragm air pump (optional) | Fills the air reservoir automatically |
| Air reservoir (~0.5L) | Stores pressurized air between sprays |
| One-way check valve | Prevents backflow from reservoir to pump |
| Pressure relief valve (≤10 PSI) | Hardware safety limit on reservoir pressure |
| 1/4" silicone tubing + fittings | Connects pump → check valve → reservoir → solenoid → nozzle |
| Wide-bore nozzle or air stone | Diffuses the air puff for cat safety |
| 12V power supply | For the solenoid/pump (adjust to your components) |
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

Relay Ch 1 (Solenoid)   Arduino Uno
────────────────────     ───────────
Signal ────────────────► D7 (DETERRENT_PIN)
VCC    ────────────────► 5V
GND    ────────────────► GND

Relay Ch 2 (Pump)       Arduino Uno
────────────────────     ───────────
Signal ────────────────► D8 (PUMP_PIN)
VCC    ────────────────► 5V
GND    ────────────────► GND

Pneumatic path:
  [12V Pump] → [Check Valve] → [Reservoir] → [Solenoid Valve] → [Nozzle]
  Relay Ch 1 NO/COM ──► Solenoid valve circuit (12V)
  Relay Ch 2 NO/COM ──► Air pump circuit (12V)
```

> **Note:** The Arduino Uno's I2C pins cannot supply enough power for HUSKYLENS 2. Connect a separate USB-C cable from HUSKYLENS 2 to a power source.
>
> **Safety:** Always install a pressure relief valve (≤10 PSI) on the reservoir. Use a wide-bore nozzle to produce a gentle, diffuse puff.

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
4. (Optional) Point the camera at a **person** and press **Button-A** to learn as **ID 3** (enables human exclusion)
5. (Optional) Use **Export Model** in settings to save your trained model

### 3. Configure

Edit `myaotron/config.h` to adjust:

- **`DETECTION_MODE`** — `MODE_DUAL_OVERLAP` (default) or `MODE_CAMERA_ONLY`
- **`MIN_CONFIDENCE`** — detection confidence threshold, 0–100 (default: `30`)
- **`DEBOUNCE_FRAMES`** — consecutive detection frames required before spraying (default: `3`)
- **`OVERLAP_THRESHOLD`** — how much the cat must overlap the surface (default: `0.3`)
- **`PERSON_EXCLUSION_ENABLED`** — suppress spray when a person is at the counter (default: `1`)
- **`DETERRENT_PIN`** — digital pin connected to your relay (default: `7`)
- **`DETERRENT_ACTIVE_LOW`** — set to `1` for active-low relay modules (default: `0`)
- **`SPRAY_DURATION_MS`** — how long each spray lasts (default: `500`ms)
- **`SPRAY_COOLDOWN_MS`** — minimum time between sprays (default: `5000`ms)
- **`PUMP_MODE`** — `PUMP_MODE_NONE` (default), `PUMP_MODE_ON_DEMAND`, or `PUMP_MODE_CONTINUOUS`
- **`PUMP_PIN`** — digital pin for pump relay (default: `8`)
- **`PUMP_ACTIVE_LOW`** — set to `1` for active-low pump relay (default: `0`)
- **`PUMP_PRE_CHARGE_MS`** — pump run time at boot to fill reservoir (default: `3000`ms)
- **`PUMP_REFILL_MS`** — pump run time after each spray to refill (default: `2000`ms)
- **`STATUS_LED_PIN`** — LED pin for visual status feedback (default: `LED_BUILTIN`, `-1` to disable)
- **`LOOP_DELAY_MS`** — delay between detection loop iterations in ms (default: `200`)
- **`WATCHDOG_ENABLED`** — hardware watchdog for auto-reset on hang (default: `1`)
- **`WATCHDOG_TIMEOUT_S`** — watchdog timeout in seconds, ESP32 only (default: `10`)
- **`RECONNECT_INTERVAL_MS`** — retry interval for HUSKYLENS reconnection (default: `3000`)

### 4. Upload

1. Open `myaotron/myaotron.ino` in Arduino IDE
2. Select your board (Arduino Uno, ESP32, etc.)
3. Upload the sketch
4. Open Serial Monitor (9600 baud) to see detection output

## Detection Logic (MODE_DUAL_OVERLAP)

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

Detection pipeline:
  1. Check confidence ≥ MIN_CONFIDENCE
  2. Check horizontal overlap ≥ 30%
  3. Check cat's bottom edge within surface bounds
  4. Check no person at counter (if exclusion enabled)
  5. Require DEBOUNCE_FRAMES consecutive positive frames
  6. → SPRAY! (non-blocking)
```

## State Machine

```
  ┌───────┐  cat detected   ┌────────────┐  N frames   ┌──────────┐
  │ IDLE  │ ──────────────► │ DEBOUNCING │ ──────────► │ SPRAYING │
  └───┬───┘                 └─────┬──────┘             └────┬─────┘
      │                       detection lost                │ timer done
      │                           │                         ▼
      │                           ▼                    ┌───────────┐  pump done  ┌──────────┐
      │                      ┌────────┐                │ REFILLING │ ──────────► │ COOLDOWN │
      │◄─────────────────────│  IDLE  │◄───────────────┘───────────┘             └────┬─────┘
      │                      └────────┘    (no pump)                                  │
      │                           ▲────────────────────────────────────────────────────┘
      │  I2C failure
      ▼
  ┌───────┐  reconnected
  │ ERROR │ ──────────────► IDLE
  └───────┘

Note: REFILLING state only active when PUMP_MODE is PUMP_MODE_ON_DEMAND.
When PUMP_MODE_NONE or PUMP_MODE_CONTINUOUS, SPRAYING goes directly to COOLDOWN.

Status LED:
  Off          = idle
  Slow blink   = debouncing (cat detected, confirming)
  Solid        = spraying
  Medium blink = refilling reservoir
  Fast blink   = HUSKYLENS error
```

## Testing

### Desktop Unit Tests

The geometry logic (bounding box overlap, vertical positioning, confidence check) is extracted into `detection_logic.h` and tested on desktop with no Arduino dependencies:

```bash
cd test && make
```

### Hardware Integration Test

Upload `test_hardware/test_hardware.ino` to verify wiring without needing an actual cat:

1. Open in Arduino IDE and upload
2. Open Serial Monitor (9600 baud)
3. The sketch cycles through:
   - **LED test** — blinks in each status pattern (verify visually)
   - **Deterrent relay test** — fires two brief pulses (listen for clicks)
   - **Pump relay test** — fires a brief pulse if pump enabled (listen for motor)
   - **HUSKYLENS test** — connects via I2C and reads a frame

Each subsystem reports PASS/FAIL on Serial.

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "HUSKYLENS 2 not found" | Check I2C wiring (SDA→A4, SCL→A5). Ensure separate USB-C power to HUSKYLENS 2 |
| Cat detected but no spray | Check relay wiring on pin D7. Open Serial Monitor to see detection output |
| Too many false positives | Increase `MIN_CONFIDENCE`, `OVERLAP_THRESHOLD`, or `DEBOUNCE_FRAMES` in config.h |
| Sprays when cat is on floor | Make sure counter is learned as ID 2. Try repositioning camera angle |
| Sprays when I'm at the counter | Learn "person" as ID 3 and ensure `PERSON_EXCLUSION_ENABLED` is `1` |
| Counter not detected | Lower the Detection Threshold in HUSKYLENS 2 settings. Re-learn the surface |
| LED blinking fast | HUSKYLENS 2 disconnected — check I2C wiring and USB-C power |
| Relay clicks but no spray | Check if your relay is active-low; set `DETERRENT_ACTIVE_LOW` to `1` |
| Pump runs but no air puff | Check valve may be installed backward. Verify tubing has no kinks |
| Pump never stops | Verify `PUMP_MODE` is not `PUMP_MODE_CONTINUOUS` unintentionally. Check `PUMP_REFILL_MS` |
| Weak air puff | Increase `PUMP_PRE_CHARGE_MS` for longer initial charge. Check for leaks in tubing/fittings |

## License

MIT
