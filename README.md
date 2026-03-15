# 🐱 myaotron

Cat-on-counter detection and deterrent system powered by [HUSKYLENS 2](https://wiki.dfrobot.com/sen0638/docs/22636/) AI camera.

Detects when your cat jumps on the kitchen counter and gives them a harmless puff of air to discourage the behavior — automatically and humanely. Uses a push solenoid to press the trigger on a [PetSafe SSSCat](https://www.petsafe.net/ssscat) compressed air can.

## How It Works

```
┌──────────┐     I2C      ┌─────────┐   D7    ┌───────┐   12V    ┌──────────┐
│HUSKYLENS │────────────►│ Arduino │────────►│ Relay │────────►│  Push    │
│    2     │              │   Uno   │         │Module │         │ Solenoid │
└──────────┘              └─────────┘         └───────┘         └────┬─────┘
                                                                     │ pushes
                                                                     ▼
                                                               ┌──────────┐
                                                               │  SSSCat  │
                                                               │   Can    │──► puff!
                                                               └──────────┘
```

The HUSKYLENS 2 uses its built-in Object Recognition (MS COCO 80 classes) to detect both **cats** and **surfaces** (dining table/counter) in the camera frame. When a cat-on-counter event is confirmed, the Arduino energizes a push solenoid that physically presses the SSSCat can's trigger button, releasing a short burst of air.

### Detection Modes

| Mode | Strategy | Best For |
|------|----------|----------|
| **MODE_DUAL_OVERLAP** (default) | Triggers only when a cat's bounding box overlaps a detected counter surface | General use, fewer false positives |
| **MODE_CAMERA_ONLY** (fallback) | Triggers on any cat detection in frame | Camera aimed exclusively at the counter |

## Parts List

### Brain (Detection)

| # | Part | Link | ~Price |
|---|------|------|--------|
| 1 | HUSKYLENS 2 (SEN0638) — AI camera | [DFRobot](https://www.dfrobot.com/product-2828.html) | $50 |
| 2 | Arduino Uno R3 (or compatible) | [DFRobot](https://www.dfrobot.com/product-838.html) / [Adafruit](https://www.adafruit.com/product/50) | $25 |
| 3 | Gravity I2C cable (4-pin) | Included with HUSKYLENS 2 | — |
| 4 | USB-C cable (for HUSKYLENS 2 power) | Any USB-C cable | $5 |
| 5 | USB-B cable (for Arduino programming/power) | Usually included with Arduino | — |

### Deterrent (Air Puff)

| # | Part | Link | ~Price |
|---|------|------|--------|
| 6 | PetSafe SSSCat refill can (3.89 oz) | [Amazon](https://www.amazon.com/dp/B0721735K9) / [PetSafe](https://www.petsafe.net/ssscat) | $10 |
| 7 | Push solenoid — 12V, 5N | [Adafruit 412](https://www.adafruit.com/product/412) | $8 |
| 8 | 1-channel relay module (5V logic, 10A) | [Adafruit 3191](https://www.adafruit.com/product/3191) | $10 |
| 9 | 12V 1A DC power supply (barrel jack) | [Adafruit 798](https://www.adafruit.com/product/798) | $9 |
| 10 | DC barrel jack to screw terminal adapter | [Adafruit 368](https://www.adafruit.com/product/368) | $2 |
| 11 | 1N4007 flyback diode | [Adafruit 755](https://www.adafruit.com/product/755) (pack of 10) | $2 |

### Misc

| # | Part | Link | ~Price |
|---|------|------|--------|
| 12 | Jumper wires (M-M and M-F) | [Adafruit 1957](https://www.adafruit.com/product/1957) | $4 |
| 13 | Small breadboard (optional, for prototyping) | [Adafruit 64](https://www.adafruit.com/product/64) | $5 |
| 14 | Mounting bracket for solenoid + can | 3D print, wood block, or zip-ties + L-bracket | $0–5 |

**Estimated total: ~$130** (plus the Arduino if you don't have one)

> **Why PetSafe SSSCat?** Regular keyboard duster cans contain difluoroethane — a propellant that is **toxic to cats** (causes CNS depression and cardiac arrhythmia). SSSCat refills are specifically formulated and tested for safe use around pets.

## Step-by-Step Wiring

> **New to electronics?** Don't worry — this project uses only simple wire connections. No soldering required if you use a breadboard and jumper wires.

### What is I2C?

I2C (pronounced "eye-squared-see") is a communication protocol that uses just two wires — **SDA** (data) and **SCL** (clock) — to let devices talk to each other. The HUSKYLENS 2 uses I2C to send detection results to the Arduino.

### What is a Relay?

A relay is an electrically controlled switch. The Arduino sends a small signal (5V) to the relay, which then switches a larger circuit (12V) on or off. This lets the low-power Arduino control the higher-power solenoid.

### Wiring Diagram

```
                    ┌─────────────────────┐
                    │     Arduino Uno     │
                    │                     │
                    │  A4 (SDA) ◄────────┼──── HUSKYLENS 2 SDA (blue wire)
                    │  A5 (SCL) ◄────────┼──── HUSKYLENS 2 SCL (yellow wire)
                    │  GND      ◄────────┼──── HUSKYLENS 2 GND (black wire)
                    │                     │
                    │  D7       ─────────┼──── Relay IN (signal)
                    │  5V       ─────────┼──── Relay VCC
                    │  GND      ─────────┼──── Relay GND
                    └─────────────────────┘

    ┌───────────┐
    │   12V PSU │
    │  via      │
    │  barrel   │──── (+) ─────────► Relay COM terminal
    │  jack     │──── (−) ─────┐
    │  adapter  │              │
    └───────────┘              │
                               │
    Relay NO terminal ─────────┼──► Solenoid wire 1
                               │
    Solenoid wire 2 ◄──────────┘

    ⚡ FLYBACK DIODE: Solder or clip a 1N4007 diode across the
    solenoid's two wires with the stripe (cathode) facing the
    positive (+) side. This protects the relay from voltage spikes.
```

### Step-by-Step

1. **Connect HUSKYLENS 2 to Arduino:**
   - Plug the included 4-pin Gravity I2C cable into the I2C port on the back of the HUSKYLENS 2.
   - Connect the wires to the Arduino:
     - **Blue** (SDA) → Arduino pin **A4**
     - **Yellow** (SCL) → Arduino pin **A5**
     - **Black** (GND) → Arduino **GND**
     - **Red** (VCC) → leave disconnected (HUSKYLENS 2 gets power from USB-C instead)
   - Plug a USB-C cable into the HUSKYLENS 2 and connect it to a USB power source (phone charger, USB hub, etc.).

2. **Connect the relay module to Arduino:**
   - **IN** (signal) → Arduino pin **D7**
   - **VCC** → Arduino **5V**
   - **GND** → Arduino **GND**

3. **Connect the 12V power supply:**
   - Plug the 12V barrel jack adapter into the power supply.
   - Connect the **+** screw terminal to the relay's **COM** (common) terminal.

4. **Connect the push solenoid:**
   - Connect one solenoid wire to the relay's **NO** (normally open) terminal.
   - Connect the other solenoid wire to the **−** screw terminal of the barrel jack adapter.
   - **Add the flyback diode:** Clip or solder a 1N4007 diode directly across the solenoid's two wires. The **silver stripe on the diode** (cathode) must face the **positive (+)** side (the wire going to the relay). This protects the relay from damage when the solenoid turns off.

5. **Power the Arduino:**
   - Connect the Arduino to your computer (or a USB power adapter) with the USB-B cable.

> **Safety note:** Double-check all connections before powering on. The 12V circuit should only power the solenoid through the relay. Never connect 12V to the Arduino's 5V or signal pins.

### Mounting the Solenoid and Can

The push solenoid needs to be aligned so its plunger presses the SSSCat can's trigger button. Options:

- **3D-printed bracket** — most precise (search Thingiverse for "aerosol solenoid mount" or design your own)
- **Wood block + zip-ties** — clamp the can and solenoid to an L-bracket or wood block so they're aligned
- **Simple test setup** — lay the can on its side in a V-groove (two books), position the solenoid by hand, tape in place

The solenoid has 5.5mm of stroke and the aerosol trigger needs about 1–2mm of travel. Leave a small gap (~3mm) between the solenoid plunger and the trigger so it's not pressing when de-energized.

> **Important:** Always mount the SSSCat can **upright** (nozzle pointing horizontally or slightly downward). Never mount it upside down — liquid propellant could escape and cause frostbite.

## Software Setup

### 1. Install Arduino IDE

Download and install [Arduino IDE](https://www.arduino.cc/en/software) (version 2.x recommended).

### 2. Install the HUSKYLENS Library

1. Download the [DFRobot_HuskylensV2](https://github.com/DFRobot/DFRobot_HuskylensV2) library as a ZIP file (Code → Download ZIP)
2. In Arduino IDE: **Sketch → Include Library → Add .ZIP Library**
3. Select the downloaded ZIP file

### 3. Train HUSKYLENS 2

Before uploading the sketch, teach HUSKYLENS 2 to recognize your cat and counter:

1. Power on HUSKYLENS 2 and select **Object Recognition** using the function button
2. Point the camera at your **cat** and press **Learn button** to learn it as **ID 1**
3. Point the camera at your **counter/table surface** and press **Learn button** to learn it as **ID 2**
4. (Optional) Point at a **person** and press **Learn button** to learn as **ID 3** (enables human exclusion — the system won't spray when you're at the counter)
5. (Optional) Go to Settings → Export Model to save your trained model

> **Tips:** Train in the same lighting conditions the camera will operate in. Train from multiple angles for better recognition. You can re-train anytime.

### 4. Configure

Edit `myaotron/config.h` to adjust settings. The defaults work well for most setups:

| Setting | Default | What it does |
|---------|---------|-------------|
| `DETECTION_MODE` | `MODE_DUAL_OVERLAP` | Detection strategy (see table above) |
| `MIN_CONFIDENCE` | `30` | Minimum confidence (0–100) to count a detection |
| `DEBOUNCE_FRAMES` | `3` | Consecutive frames needed before spraying (~600ms) |
| `OVERLAP_THRESHOLD` | `0.3` | How much the cat must overlap the surface (30%) |
| `PERSON_EXCLUSION_ENABLED` | `1` | Suppress spray when a person is at the counter |
| `DETERRENT_PIN` | `7` | Arduino pin connected to relay signal |
| `DETERRENT_ACTIVE_LOW` | `0` | Set to `1` for active-low relay modules |
| `SPRAY_DURATION_MS` | `500` | How long the solenoid pushes the trigger (ms) |
| `SPRAY_COOLDOWN_MS` | `5000` | Minimum time between sprays (ms) |
| `STATUS_LED_PIN` | `LED_BUILTIN` | LED for visual status (`-1` to disable) |
| `DEBUG_SERIAL` | `1` | Print detection info to Serial Monitor |
| `SERIAL_BAUD` | `9600` | Serial Monitor baud rate |
| `LOOP_DELAY_MS` | `200` | Delay between detection loops (ms) |
| `WATCHDOG_ENABLED` | `1` | Auto-reset if the board hangs |
| `WATCHDOG_TIMEOUT_S` | `10` | Watchdog timeout, ESP32 only (seconds) |
| `RECONNECT_INTERVAL_MS` | `3000` | Retry interval for HUSKYLENS reconnection (ms) |

### 5. Upload and Test

1. Open `myaotron/myaotron.ino` in Arduino IDE
2. Select your board: **Tools → Board → Arduino Uno** (or your board)
3. Select port: **Tools → Port** → pick the one that appeared when you plugged in the Arduino
4. Click **Upload** (→ arrow button)
5. Open **Serial Monitor** (magnifying glass icon, top right) — set baud to **9600**
6. You should see: `myaotron starting...` then `Ready — watching for cats on counter...`

### Running the Hardware Test

Before deploying, test each component individually:

1. Open `test_hardware/test_hardware.ino` in Arduino IDE
2. Upload it to the Arduino
3. Open Serial Monitor (9600 baud)
4. The sketch tests each subsystem:
   - **LED test** — blinks in each status pattern (idle, debounce, spray, error)
   - **Relay test** — fires two brief pulses (you should hear clicks and see the solenoid extend)
   - **HUSKYLENS test** — connects via I2C and reads a frame

Each test reports **PASS** or **FAIL**.

## Camera Placement Tips

- Mount the camera 2–3 feet above the counter, angled down ~45°
- Make sure the entire counter surface is in frame
- Avoid pointing at windows (bright backlight hurts detection)
- Position the SSSCat can so its nozzle points across the counter at cat-jump height
- Keep the can 1–3 feet from where cats typically land

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
  6. → Activate solenoid (non-blocking)
```

## State Machine

```
  ┌───────┐  cat detected   ┌────────────┐  N frames   ┌──────────┐
  │ IDLE  │ ──────────────► │ DEBOUNCING │ ──────────► │ SPRAYING │
  └───┬───┘                 └─────┬──────┘             └────┬─────┘
      │                       detection lost                │ timer done
      │                           │                         ▼
      │                           ▼                    ┌──────────┐
      │                      ┌────────┐                │ COOLDOWN │
      │◄─────────────────────│  IDLE  │◄───────────────┘──────────┘
      │                      └────────┘
      │  I2C failure
      ▼
  ┌───────┐  reconnected
  │ ERROR │ ──────────────► IDLE
  └───────┘

Status LED:
  Off        = idle (no detection) or cooldown
  Slow blink = debouncing (cat detected, confirming)
  Solid      = spraying (solenoid active)
  Fast blink = HUSKYLENS error
```

## Testing

### Desktop Unit Tests

The geometry logic is extracted into `detection_logic.h` and tested on desktop with no Arduino dependencies:

```bash
cd test && make
```

Runs 19 tests covering bounding box overlap, vertical positioning, confidence checks, and edge cases.

### Hardware Integration Test

Upload `test_hardware/test_hardware.ino` to verify wiring without needing an actual cat — see [Running the Hardware Test](#running-the-hardware-test) above.

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "HUSKYLENS 2 not found" | Check I2C wiring (SDA→A4, SCL→A5). Ensure separate USB-C power to HUSKYLENS 2 |
| Cat detected but no spray | Check relay wiring on D7. Verify 12V power to solenoid. Open Serial Monitor |
| Solenoid clicks but can doesn't spray | Adjust solenoid alignment — plunger may not be reaching the trigger. Reduce the gap |
| Solenoid doesn't click at all | Check 12V PSU and diode orientation (stripe toward +). Try swapping relay NO/NC |
| Too many false positives | Increase `MIN_CONFIDENCE`, `OVERLAP_THRESHOLD`, or `DEBOUNCE_FRAMES` in config.h |
| Sprays when cat is on floor | Make sure counter is learned as ID 2. Reposition camera angle |
| Sprays when I'm at counter | Learn "person" as ID 3 and ensure `PERSON_EXCLUSION_ENABLED` is `1` |
| Counter not detected | Lower the Detection Threshold in HUSKYLENS 2 settings. Re-learn the surface |
| LED blinking fast | HUSKYLENS 2 disconnected — check I2C wiring and USB-C power |
| Relay clicks but solenoid doesn't fire | Check if your relay is active-low; set `DETERRENT_ACTIVE_LOW` to `1` |
| SSSCat can runs out fast | Increase `SPRAY_COOLDOWN_MS` to reduce spray frequency. Each can has ~120 sprays |

## Safety Notes

- **Never use keyboard duster cans** — they contain difluoroethane, which is toxic to cats
- **Always mount the SSSCat can upright** — inverted cans can release liquid propellant (frostbite risk)
- **The flyback diode is required** — without it, the relay contacts will arc and degrade when the solenoid turns off
- **Keep 12V away from Arduino** — only the solenoid circuit uses 12V; the Arduino runs on 5V via USB
- **Supervise initial runs** — watch the first few activations to confirm the spray is gentle and aimed correctly

## License

MIT
