# 🐱 myaotron

Cat-on-counter detection and deterrent system powered by [HUSKYLENS 2](https://wiki.dfrobot.com/sen0638/docs/22636/) AI camera.

Detects when your cat jumps on the kitchen counter and gives them a harmless puff of air to discourage the behavior — automatically and humanely. Hacks a [PetSafe SSSCat](https://www.petsafe.net/ssscat) spray device, replacing its basic motion sensor with AI-powered cat detection.

## How It Works

```
┌──────────┐       ┌─────────┐       ┌───────┐       ┌──────────┐
│HUSKYLENS │─I2C──►│ Arduino │──D7──►│ Relay │──6V──►│  SSSCat  │
│    2     │       │   Uno   │       │Module │       │ (hacked) │──► puff!
└──────────┘       └─────────┘       └───────┘       └──────────┘
```

The HUSKYLENS 2 uses its built-in Object Recognition (MS COCO 80 classes) to detect both **cats** and **surfaces** (dining table/counter) in the camera frame. When a cat-on-counter event is confirmed, the Arduino closes a relay that powers the SSSCat's internal solenoid via an external battery pack, releasing a short burst of air — just like the original device, but triggered by AI instead of a basic motion sensor.

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
| 6 | PetSafe SSSCat **complete device** (includes one refill can) | [Amazon](https://www.amazon.com/dp/B000RIA95G) / [PetSafe](https://www.petsafe.net/ssscat) | $25 |
| 7 | 1-channel relay module (5V logic, 10A) | [Arduino Official](https://store.arduino.cc/products/1-relay-module-5-vdc-10a-assembled) | $6 |
| 8 | 4× AAA batteries | Any brand (for the SSSCat's built-in battery compartment) | $3 |
| 9 | 1N4007 flyback diode | [Adafruit 755](https://www.adafruit.com/product/755) (pack of 10) | $2 |

### Misc

| # | Part | Link | ~Price |
|---|------|------|--------|
| 10 | Jumper wires (M-M and M-F) | [Adafruit 1957](https://www.adafruit.com/product/1957) | $4 |
| 11 | Small breadboard (optional, for prototyping) | [Adafruit 64](https://www.adafruit.com/product/64) | $5 |
| 12 | Wire nuts or small lever connectors (2-pack) | Hardware store electrical aisle | $2 |

**Estimated total: ~$100** (plus the Arduino if you don't have one)

> **Why PetSafe SSSCat?** Regular keyboard duster cans contain difluoroethane — a propellant that is **toxic to cats** (causes CNS depression and cardiac arrhythmia). SSSCat refills are specifically formulated and tested for safe use around pets.

## Step-by-Step Wiring

> **New to electronics?** Don't worry — this project uses only simple wire connections. No soldering required if you use a breadboard and jumper wires.

### What is I2C?

I2C (pronounced "eye-squared-see") is a communication protocol that uses just two wires — **SDA** (data) and **SCL** (clock) — to let devices talk to each other. The HUSKYLENS 2 uses I2C to send detection results to the Arduino.

### What is a Relay?

A relay is an electrically controlled switch. The Arduino sends a small signal (5V) to the relay, which then switches a separate circuit (6V batteries) on or off. This lets the low-power Arduino control the SSSCat's solenoid.

### What is a Flyback Diode?

When you turn off an electromagnet (like the solenoid inside the SSSCat), the collapsing magnetic field creates a brief voltage spike that can damage other components. A flyback diode absorbs this spike safely. It's a small component with a stripe on one end (marking the direction). You'll connect it across the SSSCat solenoid wires — see the [wiring steps](#step-by-step) below for exactly how.

### Wiring Diagram

```
                    ┌─────────────────────┐
                    │     Arduino Uno     │
                    │                     │
                    │  A4 (SDA) ◄─────────┼──── HUSKYLENS 2 SDA (blue wire)
                    │  A5 (SCL) ◄─────────┼──── HUSKYLENS 2 SCL (yellow wire)
                    │  GND      ◄─────────┼──── HUSKYLENS 2 GND (black wire)
                    │                     │
                    │  D7       ──────────┼──── Relay IN (signal)
                    │  5V       ──────────┼──── Relay VCC
                    │  GND      ──────────┼──── Relay GND
                    └─────────────────────┘

    6V Battery Circuit (uses SSSCat's built-in battery compartment):

    Two wires exit the SSSCat housing (see "Modifying the SSSCat"):
      Wire A = battery (+)
      Wire B = solenoid wire 1

    Inside the SSSCat, battery (−) and solenoid wire 2 are
    connected together, and the flyback diode is across the
    solenoid. So only 2 wires come out:

    Wire A ──────────► Relay COM
                           │
                       Relay NO
                           │
    Wire B ◄───────────────┘
```

> **Note about wire colors:** The colors listed above (blue, yellow, black, red) are for the DFRobot Gravity I2C cable included with the HUSKYLENS 2. If you use a different I2C cable, check its documentation for the correct SDA/SCL/GND/VCC wires.

### Step-by-Step

1. **Connect HUSKYLENS 2 to Arduino:**
   - Plug the included 4-pin Gravity I2C cable into the I2C port on the back of the HUSKYLENS 2.
   - Connect the wires to the Arduino:
     - **Blue** (SDA) → Arduino pin **A4**
     - **Yellow** (SCL) → Arduino pin **A5**
     - **Black** (GND) → Arduino **GND**
     - **Red** (VCC) → leave disconnected (HUSKYLENS 2 gets power from USB-C instead). Tuck the bare end out of the way with a small piece of tape so it can't accidentally touch anything.
   - Plug a USB-C cable into the HUSKYLENS 2 and connect it to a USB power source (phone charger, USB hub, etc.).

2. **Connect the relay module to Arduino:**
   - **IN** (signal) → Arduino pin **D7**
   - **VCC** → Arduino **5V**
   - **GND** → Arduino **GND**

3. **Connect the two SSSCat wires to the relay:**
   - Connect **Wire A** (battery +) to the relay's **COM** (common) terminal.
   - Connect **Wire B** (solenoid wire 1) to the relay's **NO** (normally open) terminal.
   - That's it — the battery (−), solenoid wire 2, and flyback diode are all connected inside the SSSCat housing (see [Modifying the SSSCat](#modifying-the-ssscat) below).

4. **Power the Arduino:**
   - Connect the Arduino to your computer (or a USB power adapter) with the USB-B cable.

> **Safety note:** Double-check all connections before powering on. The 6V battery circuit should only power the SSSCat solenoid through the relay. Never connect the batteries to the Arduino's 5V or signal pins.

### Modifying the SSSCat

> **⚠️ These steps have not been verified on an actual SSSCat unit yet.** The
> general approach (disconnect solenoid from PCB, route wires out) is confirmed
> by [community projects](https://forum.arduino.cc/t/ssscat-arduino-hack/363398),
> but internal layout may vary between SSSCat models and revisions. Open your
> unit and inspect it before cutting anything. Photos of your specific internals
> are welcome — we'll update these steps once verified.

You'll open the SSSCat device, disconnect its internal solenoid from the built-in
motion sensor PCB, and route the solenoid and battery wires out so your Arduino
can control them.

**Tools needed:** Small Phillips screwdriver, wire cutters/strippers, and either
wire nuts, lever connectors (like Wago 221), or a soldering iron.

**Steps:**

1. **Remove the can.** Twist the SSSCat's top unit (the part with the sensor
   and nozzle) off the refill can. Set the can aside — you'll put it back later.

2. **Open the top unit.** Remove the screws holding the housing together (usually
   2–4 small Phillips screws on the bottom or sides). Gently pry the two halves
   apart. You'll see a small PCB (circuit board) with the motion sensor, a
   battery compartment, and the solenoid — a small metal cylinder near the
   spray nozzle.

3. **Find the solenoid wires.** The solenoid has **two wires** running to the
   PCB. They may be soldered directly or connected with a small plug. Follow
   them from the metal solenoid body to where they attach to the circuit board.

4. **Disconnect the solenoid from the PCB.** If the wires have a plug, simply
   unplug it. If soldered, clip the wires close to the PCB (not close to the
   solenoid — you want to keep as much wire length as possible).

5. **Find the battery wires.** The battery compartment also has **two wires**
   going to the PCB — a red (+) and black (−). Cut these from the PCB too,
   keeping as much wire length as possible.

6. **Make the internal connections.** Inside the housing, connect these three
   things together:
   - **Battery (−) wire** to **solenoid wire 2** (either solenoid wire — pick one
     and call it "wire 2"). Use a wire nut or lever connector.
   - **Flyback diode** across the two solenoid wires. The **silver stripe**
     (cathode) must face solenoid wire 1 (the one NOT connected to the battery).
     Twist or solder the diode leads onto the solenoid wires.

7. **Extend the two outgoing wires.** You need two wires to reach the relay
   (about 12–18 inches):
   - **Wire A** = battery (+) wire
   - **Wire B** = solenoid wire 1 (the free one, not connected to battery −)

   If the existing wires are long enough, great. If not, splice on extension
   wires using wire nuts or lever connectors. Strip about 1/4" of insulation
   from each end before connecting.

8. **Route the wires out.** Drill or cut a small hole (~1/4") in the housing
   wall (pick a spot that won't interfere with the can or nozzle). Thread
   Wire A and Wire B through the hole.

9. **Reassemble the housing.** Put the two halves back together and replace the
   screws. The solenoid and flyback diode stay inside, perfectly aligned — 
   exactly where PetSafe designed them to be.

10. **Insert batteries and snap the can back on.** Put 4× AAA batteries into the
    SSSCat's battery compartment (the compartment door is still accessible from
    outside). Then twist the refill can back into the top unit.

> **The SSSCat's PIR sensor and PCB are now inert.** With both the solenoid and
> batteries disconnected from the PCB, the original motion sensor has no power
> and can't trigger anything. Your Arduino controls everything through the relay.

## Software Setup

### 1. Install Arduino IDE

Download and install [Arduino IDE](https://www.arduino.cc/en/software) (version 2.x recommended).

### 2. Install the HUSKYLENS Library

1. Download the [DFRobot_HuskylensV2](https://github.com/DFRobot/DFRobot_HuskylensV2) library as a ZIP file (Code → Download ZIP)
2. In Arduino IDE: **Sketch → Include Library → Add .ZIP Library**
3. Select the downloaded ZIP file

### 3. Test Your Wiring (Hardware Test)

Before training the camera or uploading the main sketch, verify that your wiring is correct:

1. Open `test_hardware/test_hardware.ino` in Arduino IDE
2. Select your board: **Tools → Board → Arduino Uno** (or your board)
3. Select port: **Tools → Port** → pick the one that appeared when you plugged in the Arduino
4. Click **Upload** (→ arrow button)
5. Open **Serial Monitor** (magnifying glass icon, top right) — set baud to **9600**
6. The sketch tests each subsystem:
   - **LED test** — blinks in each status pattern (idle, debounce, spray, error)
   - **Relay test** — fires two brief pulses (you should hear clicks and see the solenoid extend)
   - **HUSKYLENS test** — connects via I2C and reads a frame

Each test reports **PASS** or **FAIL**. Fix any failures before continuing.

### 4. Train HUSKYLENS 2

With the HUSKYLENS 2 powered on via USB-C (from the wiring step), teach it to recognize your cat and counter:

1. On the HUSKYLENS 2 screen, select **Object Recognition** using the **function button** (the small button on the top edge of the device, next to the dial)
2. Point the camera at your **cat** and press the **Learn button** (the large button on the top edge) to learn it as **ID 1**
3. Point the camera at your **counter/table surface** and press **Learn button** to learn it as **ID 2**
4. (Optional) Point at a **person** and press **Learn button** to learn as **ID 3** (enables human exclusion — the system won't spray when you're at the counter)
5. (Optional) Go to Settings → Export Model to save your trained model

> **Tips:** Train in the same lighting conditions the camera will operate in. Train from multiple angles for better recognition. You can re-train anytime.

### 5. Configure

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
| `SPRAY_COOLDOWN_MS` | `5000` | Minimum time between sprays (ms). Keep ≥2000 to avoid solenoid overheating |
| `MAX_SPRAYS_PER_HOUR` | `20` | Safety limit — pauses spraying if exceeded. `0` to disable |
| `CAN_CAPACITY_SPRAYS` | `120` | Approximate sprays per SSSCat can. Prints serial warning near depletion |
| `STATUS_LED_PIN` | `LED_BUILTIN` | LED for visual status (`-1` to disable) |
| `DEBUG_SERIAL` | `1` | Print detection info to Serial Monitor |
| `SERIAL_BAUD` | `9600` | Serial Monitor baud rate |
| `LOOP_DELAY_MS` | `200` | Delay between detection loops (ms) |
| `WATCHDOG_ENABLED` | `1` | Auto-reset if the board hangs |
| `WATCHDOG_TIMEOUT_S` | `10` | Watchdog timeout, ESP32 only (seconds) |
| `RECONNECT_INTERVAL_MS` | `3000` | Retry interval for HUSKYLENS reconnection (ms) |

### 6. Upload the Main Sketch

1. Open `myaotron/myaotron.ino` in Arduino IDE
2. Click **Upload** (→ arrow button)
3. Open **Serial Monitor** (9600 baud)
4. You should see: `myaotron starting...` then `Ready — watching for cats on counter...`

## Camera Placement Tips

- Mount the camera 2–3 feet above the counter, angled down ~45°. A kitchen cabinet underside, a shelf, or a small clamp (like a spring clamp or phone mount) all work well. Adhesive command strips can hold the lightweight HUSKYLENS in place without drilling.
- Make sure the entire counter surface is in frame
- Avoid pointing at windows (bright backlight hurts detection)
- Position the SSSCat device so its nozzle points across the counter at cat-jump height
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

Upload `test_hardware/test_hardware.ino` to verify wiring without needing an actual cat — see [Step 3: Test Your Wiring](#3-test-your-wiring-hardware-test) above.

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "HUSKYLENS 2 not found" | Check I2C wiring (SDA→A4, SCL→A5). Ensure separate USB-C power to HUSKYLENS 2 |
| Cat detected but no spray | Check relay wiring on D7. Verify SSSCat has fresh batteries. Open Serial Monitor |
| Solenoid clicks but can doesn't spray | Can may not be seated properly — twist it firmly into the SSSCat housing. Or solenoid wires may be loose |
| Solenoid doesn't click at all | Check SSSCat battery compartment (fresh AAA batteries?). Check diode orientation (stripe toward relay side). Try swapping relay NO/NC |
| Too many false positives | Increase `MIN_CONFIDENCE`, `OVERLAP_THRESHOLD`, or `DEBOUNCE_FRAMES` in config.h |
| Sprays when cat is on floor | Make sure counter is learned as ID 2. Reposition camera angle |
| Sprays when I'm at counter | Learn "person" as ID 3 and ensure `PERSON_EXCLUSION_ENABLED` is `1` |
| Counter not detected | Lower the Detection Threshold in HUSKYLENS 2 settings. Re-learn the surface |
| LED blinking fast | HUSKYLENS 2 disconnected — check I2C wiring and USB-C power |
| Relay clicks but solenoid doesn't fire | Check SSSCat has fresh AAA batteries. Check if relay is active-low; set `DETERRENT_ACTIVE_LOW` to `1` |
| SSSCat can runs out fast | Increase `SPRAY_COOLDOWN_MS` or lower `MAX_SPRAYS_PER_HOUR`. Each can has ~120 sprays |
| "Rate limit reached" message | Too many sprays in one hour — check for false positives. Resets automatically after 1 hour |
| "Can depleted" message | Replace the SSSCat refill can and reset the Arduino to zero the spray counter |

## Safety Notes

- **Never use keyboard duster cans** — they contain difluoroethane, which is toxic to cats
- **Always mount the SSSCat can upright** — inverted cans can release liquid propellant (frostbite risk)
- **The flyback diode is required** — without it, the relay contacts will arc and degrade when the solenoid turns off
- **Keep battery circuit separate from Arduino** — the SSSCat's 6V batteries power only the solenoid through the relay; the Arduino runs on 5V via USB
- **Supervise initial runs** — watch the first few activations to confirm the spray is gentle and aimed correctly
- **Test after reassembly** — after modifying the SSSCat, snap the can back in and test with the hardware test sketch before deploying
- **Cats with severe asthma** — SSSCat is safe for most cats, but if your cat has a respiratory condition, consult your vet before using any air-spray deterrent

## License

MIT
