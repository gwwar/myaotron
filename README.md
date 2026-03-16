# 🐱 myaotron

Cat-on-counter detection and deterrent system powered by [HUSKYLENS 2](https://wiki.dfrobot.com/sen0638/docs/22636/) AI camera.

Detects when your cat jumps on the kitchen counter and gives them a harmless puff of air to discourage the behavior — automatically and humanely. Uses a push solenoid to press the trigger on a [PetSafe SSSCat](https://www.petsafe.net/ssscat) compressed air can.

## How It Works

```
┌──────────┐       ┌─────────┐       ┌───────┐       ┌──────────┐
│HUSKYLENS │─I2C──►│ Arduino │──D7──►│ Relay │─12V──►│  Push    │
│    2     │       │   Uno   │       │Module │       │ Solenoid │
└──────────┘       └─────────┘       └───────┘       └────┬─────┘
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
| 7 | Push solenoid — 12V | [Adafruit 413](https://www.adafruit.com/product/413) | $10 |
| 8 | 1-channel relay module (5V logic, 10A) | [Arduino Official](https://store.arduino.cc/products/1-relay-module-5-vdc-10a-assembled) | $6 |
| 9 | 12V 1A DC power supply (barrel jack) | [Adafruit 798](https://www.adafruit.com/product/798) | $9 |
| 10 | DC barrel jack to screw terminal adapter | [Adafruit 368](https://www.adafruit.com/product/368) | $2 |

> **What's a barrel jack adapter?** The 12V power supply has a round plug (barrel jack) on the end. This small adapter lets you plug it in and exposes **+** and **−** screw terminals so you can attach wires to it.
| 11 | 1N4007 flyback diode | [Adafruit 755](https://www.adafruit.com/product/755) (pack of 10) | $2 |

### Misc

| # | Part | Link | ~Price |
|---|------|------|--------|
| 12 | Jumper wires (M-M and M-F) | [Adafruit 1957](https://www.adafruit.com/product/1957) | $4 |
| 13 | Small breadboard (optional, for prototyping) | [Adafruit 64](https://www.adafruit.com/product/64) | $5 |
| 14 | Mounting bracket materials | Scrap wood (1×2 lumber), hose clamp, #6 wood screws, rubber bumper pad — see [Mounting](#mounting-the-solenoid-and-can) | $0–5 |

**Estimated total: ~$128** (plus the Arduino if you don't have one)

> **Why PetSafe SSSCat?** Regular keyboard duster cans contain difluoroethane — a propellant that is **toxic to cats** (causes CNS depression and cardiac arrhythmia). SSSCat refills are specifically formulated and tested for safe use around pets.

## Step-by-Step Wiring

> **New to electronics?** Don't worry — this project uses only simple wire connections. No soldering required if you use a breadboard and jumper wires.

### What is I2C?

I2C (pronounced "eye-squared-see") is a communication protocol that uses just two wires — **SDA** (data) and **SCL** (clock) — to let devices talk to each other. The HUSKYLENS 2 uses I2C to send detection results to the Arduino.

### What is a Relay?

A relay is an electrically controlled switch. The Arduino sends a small signal (5V) to the relay, which then switches a larger circuit (12V) on or off. This lets the low-power Arduino control the higher-power solenoid.

### What is a Solenoid?

A solenoid is an electromagnet with a metal plunger inside. When you apply power, the plunger pushes out (or pulls in). When power is removed, a spring pushes it back. In this project, the solenoid's plunger pushes the SSSCat can's trigger button.

### What is a Flyback Diode?

When you turn off an electromagnet (like a solenoid or relay coil), the collapsing magnetic field creates a brief voltage spike that can damage other components. A flyback diode absorbs this spike safely. It's a small component with a stripe on one end (marking the direction). You'll clip or solder it across the solenoid's two wires — the README will tell you exactly how.

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

    12V Power Supply Circuit (via barrel jack adapter):

    PSU (+) ─────────► Relay COM
                           │
                       Relay NO
                           │
                    Solenoid wire 1
                           │
                     [ SOLENOID ]
                           │
                    Solenoid wire 2
                           │
    PSU (−) ◄──────────────┘

    FLYBACK DIODE: Clip or solder a 1N4007 diode across the
    solenoid's two wires (wire 1 and wire 2). The silver stripe
    on the diode (cathode) must face toward wire 1 (the side
    connected to the relay). See "What is a Flyback Diode?" above.
```

> **Note about wire colors:** The colors listed above (blue, yellow, black, red) are for the DFRobot Gravity I2C cable included with the HUSKYLENS 2. If you use a different I2C cable, check its documentation for the correct SDA/SCL/GND/VCC wires.

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
   - Plug the power supply's round barrel plug into the barrel jack adapter.
   - You now have **+** and **−** screw terminals exposed on the adapter.
   - Connect the **+** screw terminal to the relay's **COM** (common) terminal using a jumper wire.

4. **Connect the push solenoid:**
   - Connect one solenoid wire to the relay's **NO** (normally open) terminal.
   - Connect the other solenoid wire to the **−** screw terminal of the barrel jack adapter.
   - **Add the flyback diode** (see "What is a Flyback Diode?" above): Clip or solder a 1N4007 diode directly across the solenoid's two wires. The **silver stripe on the diode** (cathode) must face the wire that goes to the relay (wire 1). This protects the relay from voltage spikes when the solenoid turns off.

5. **Power the Arduino:**
   - Connect the Arduino to your computer (or a USB power adapter) with the USB-B cable.

> **Safety note:** Double-check all connections before powering on. The 12V circuit should only power the solenoid through the relay. Never connect 12V to the Arduino's 5V or signal pins.

### Mounting the Solenoid and Can

The solenoid must push **straight down** onto the SSSCat can's valve stem.
The can is ~6" tall and ~2" wide, so a simple wood bracket works well.

#### Mounting Materials

| Item | Purpose | Where to get it |
|------|---------|-----------------|
| Base board, ~8" × 5" | Stable platform | Scrap wood, cutting board, or thick acrylic |
| Wood post, ~7" × 1.5" × 0.75" | Vertical support (taller than the can) | 1×2 lumber from any hardware store |
| Wood arm, ~4" × 1.5" × 0.75" | Horizontal arm over the can | Same 1×2 lumber |
| Hose clamp, 1.5"–2.5" range | Holds the can to the base board | Hardware store plumbing aisle |
| 2× M3 bolts (8–10mm) + nuts | Bolt the solenoid to the arm | Hardware store fastener aisle |
| Small rubber bumper pad | Glue to plunger tip for grip | Hardware store — self-adhesive door/furniture bumpers |
| Wood screws, #6 × 1.5" (×4) | Assemble post, arm, and hose clamp | Hardware store |

#### How to Build It

```
    Side view:

    ┌──┬═══════════════┐ ◄── arm (~4" long)
    │  │  ┌──────────┐ │
    │  │  │ SOLENOID │ │ ◄── bolted underneath arm
    │  │  │    ||    │ │
    │  │  └────||────┘ │
    │  │      (●)      │ ◄── rubber bumper on plunger tip
    │  │    ~3mm gap   │
    │  │  ┌──────────┐ │
    │  │  │  SSSCat  │ │ ◄── held upright by hose clamp
    │  │  │   can    │ │
    │  │  │          │ │
    │  │  └──────────┘ │
    └──┘                 
     ▲                   
     post (~7")          
                         
    ════════════════════ base board
```

**Steps:**

1. **Screw the post to the base board.** Stand the 7" post vertically at one end
   of the board and drive two wood screws up through the bottom of the board
   into the post. The post must be taller than the can (~6").

2. **Attach the arm to the post.** Screw the 4" arm horizontally to the top of
   the post, forming an upside-down "L". The arm should extend out over where
   the can will sit.

3. **Bolt the solenoid to the arm.** Drill two holes through the arm that line
   up with the solenoid's M3 mounting holes (on the flat end of the solenoid
   body). Bolt the solenoid to the **underside** of the arm so the plunger
   points straight down.

4. **Add a rubber bumper to the plunger tip.** Peel and stick a small rubber
   bumper pad (the kind used on cabinet doors) onto the end of the plunger.
   This prevents the metal plunger from slipping off the narrow valve stem
   and cushions repeated impacts.

5. **Clamp the can to the base board.** Stand the SSSCat refill can upright
   under the solenoid. Wrap the hose clamp around the can and screw through
   the base board (or use a zip-tie cinched to a screw in the board). The can
   should not wobble or slide when the solenoid fires.

6. **Adjust the gap.** With the solenoid unpowered, the rubber bumper on the
   plunger tip should hover ~3mm above the can's valve stem. If it's too high
   or too low, loosen the hose clamp and slide the can, or add a thin shim
   (washer) between the solenoid and the arm. The solenoid has 10mm of stroke,
   so after crossing the 3mm gap it still has plenty of travel to press the
   valve (~1–2mm of valve travel needed).

7. **Test.** Power the solenoid briefly (see Hardware Test below). You should
   hear a click and feel a puff of air. If the can hisses but doesn't spray
   well, the plunger may be off-center — loosen and re-align. If no spray at
   all, reduce the gap.

> **Important:** Always keep the SSSCat can **upright** (nozzle pointing
> horizontally or slightly downward). Never mount it upside down — liquid
> propellant could escape and cause frostbite.
>
> **Overstroke is OK.** The solenoid has more travel (10mm) than the valve
> needs (1–2mm). This is fine — aerosol valves have internal mechanical stops,
> and the solenoid's force (8–10N at contact) is within normal hand-press
> range. The SSSCat's own built-in solenoid works the same way.

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

1. On the HUSKYLENS 2 screen, select **Object Recognition** using the function button
2. Point the camera at your **cat** and press **Learn button** to learn it as **ID 1**
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

- Mount the camera 2–3 feet above the counter, angled down ~45°
- Make sure the entire counter surface is in frame
- Avoid pointing at windows (bright backlight hurts detection)
- Position the SSSCat can + mount so the nozzle points across the counter at cat-jump height
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
| Cat detected but no spray | Check relay wiring on D7. Verify 12V power to solenoid. Open Serial Monitor |
| Solenoid clicks but can doesn't spray | Plunger not centered on valve — re-align the bracket. Or gap too large — reduce to ~2mm |
| Solenoid doesn't click at all | Check 12V PSU and diode orientation (stripe toward +). Try swapping relay NO/NC |
| Too many false positives | Increase `MIN_CONFIDENCE`, `OVERLAP_THRESHOLD`, or `DEBOUNCE_FRAMES` in config.h |
| Sprays when cat is on floor | Make sure counter is learned as ID 2. Reposition camera angle |
| Sprays when I'm at counter | Learn "person" as ID 3 and ensure `PERSON_EXCLUSION_ENABLED` is `1` |
| Counter not detected | Lower the Detection Threshold in HUSKYLENS 2 settings. Re-learn the surface |
| LED blinking fast | HUSKYLENS 2 disconnected — check I2C wiring and USB-C power |
| Relay clicks but solenoid doesn't fire | Check if your relay is active-low; set `DETERRENT_ACTIVE_LOW` to `1` |
| SSSCat can runs out fast | Increase `SPRAY_COOLDOWN_MS` or lower `MAX_SPRAYS_PER_HOUR`. Each can has ~120 sprays |
| "Rate limit reached" message | Too many sprays in one hour — check for false positives. Resets automatically after 1 hour |
| "Can depleted" message | Replace the SSSCat refill can and reset the Arduino to zero the spray counter |

## Safety Notes

- **Never use keyboard duster cans** — they contain difluoroethane, which is toxic to cats
- **Always mount the SSSCat can upright** — inverted cans can release liquid propellant (frostbite risk)
- **The flyback diode is required** — without it, the relay contacts will arc and degrade when the solenoid turns off
- **Keep 12V away from Arduino** — only the solenoid circuit uses 12V; the Arduino runs on 5V via USB
- **Supervise initial runs** — watch the first few activations to confirm the spray is gentle and aimed correctly
- **Test solenoid alignment carefully** — the plunger must press straight down onto the valve stem. See [Mounting the Solenoid and Can](#mounting-the-solenoid-and-can) for gap and alignment details
- **Cats with severe asthma** — SSSCat is safe for most cats, but if your cat has a respiratory condition, consult your vet before using any air-spray deterrent

## License

MIT
