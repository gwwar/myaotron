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
| 11 | 1N4007 flyback diode | [Adafruit 755](https://www.adafruit.com/product/755) (pack of 10) | $2 |

> **What's a barrel jack adapter?** The 12V power supply has a round plug (barrel jack) on the end. This small adapter lets you plug it in and exposes **+** and **−** screw terminals so you can attach wires to it.

### Misc

| # | Part | Link | ~Price |
|---|------|------|--------|
| 12 | Jumper wires (M-M and M-F) | [Adafruit 1957](https://www.adafruit.com/product/1957) | $4 |
| 13 | Small breadboard (optional, for prototyping) | [Adafruit 64](https://www.adafruit.com/product/64) | $5 |
| 14 | Mounting bracket materials | Corner braces, hose clamp, 1×2 lumber, M3×25mm bolts, rubber bumper pads — see [Mounting](#mounting-the-solenoid-and-can) | $5–10 |

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

When you turn off an electromagnet (like a solenoid or relay coil), the collapsing magnetic field creates a brief voltage spike that can damage other components. A flyback diode absorbs this spike safely. It's a small component with a stripe on one end (marking the direction). You'll clip or solder it across the solenoid's two wires — see the [wiring steps](#step-by-step) below for exactly how.

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
     - **Red** (VCC) → leave disconnected (HUSKYLENS 2 gets power from USB-C instead). Tuck the bare end out of the way with a small piece of tape so it can't accidentally touch anything.
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
Below is a simple wood bracket you can build with basic tools and hardware-store
parts. No woodworking experience needed — all joints use small metal corner
braces, so nothing depends on tricky screw techniques.

#### What You'll Need

**Tools** (borrow these if you don't have them):

- Phillips screwdriver (or a drill with a Phillips bit)
- Drill with a 1/8" (3mm) bit — for pilot holes and solenoid bolt holes
- Pencil and ruler or tape measure
- Superglue (a tiny tube is fine)

**Mounting Materials:**

| # | Item | Purpose | Where to find it |
|---|------|---------|------------------|
| 1 | Base board, ~10" × 6", at least 1/2" thick | Stable platform | Scrap plywood, a cutting board, or ask the hardware store to cut one |
| 2 | 1×2 lumber, ~14" long (you'll cut two pieces) | Post + arm | Hardware store lumber aisle — ask them to cut a 7.5" piece and a 5" piece |
| 3 | 3× small corner braces (~1.5" × 1.5"), with screws | Joins wood pieces together strongly | Hardware store fastener/bracket aisle, usually sold in packs of 4 |
| 4 | Hose clamp, 1.5"–2.5" range | Holds the can upright | Hardware store plumbing aisle |
| 5 | 2× M3 × 25mm bolts + 2× M3 washers | Bolt solenoid to arm (threads into solenoid — no nuts needed) | Hardware store fastener aisle (metric drawer) |
| 6 | Small thin rubber bumper pads (~1–2mm thick) | Plunger tip grip | Self-adhesive clear "furniture dots" from hardware store |
| 7 | 4× rubber or felt furniture pads | Protect counter, add grip | Self-adhesive pads, usually near the bumpers |
| 8 | 2–3 small zip-ties or cable clips | Secure the solenoid wires | Hardware store electrical aisle |

> **What's a corner brace?** A small L-shaped piece of metal with screw holes.
> You screw one side to one piece of wood and the other side to the other piece.
> It makes a strong right-angle joint without any skill — just line it up and
> drive the screws. They usually come with their own short screws.

#### How to Build It

```
    Side view:

    ┌──┬═══════════════════┐ ◄── arm (~5")
    │  │     ┌──────────┐  │
    │╔═╡     │ SOLENOID │  │ ◄── bolted under arm (M3 bolts)
    │║ │     │    ||    │  │
    │║ │     └────||────┘  │
    │║ │         (●)       │ ◄── rubber bumper on tip
    │║ │       ~5mm gap    │
    │║ │     ┌──────────┐  │
    │║ │     │  SSSCat  │  │ ◄── held by hose clamp
    │║ │     │   can    │  │
    │║ │     │          │  │
    │╚═╡     └──────────┘  │
    └──┘                    
     ▲  ▲                   
     │  └── corner braces (marked ║)
     post (~7.5")           
                            
    ════════════════════════ base board + rubber feet
```

**Prep — attach the solenoid to the arm before building the bracket:**

1. Place the solenoid flat-end-down on the 5" arm piece. The flat end has M3
   threaded holes (small screw holes in the metal).

2. Line up two of the solenoid's holes with the arm. Mark the two spots on the
   wood with a pencil.

3. Drill through the arm at both marks with the 1/8" (3mm) bit.

4. Flip the arm over. Push an M3 × 25mm bolt down through each hole from the
   top of the arm. Place a washer under each bolt head (this spreads the load
   so the bolt doesn't dig into the wood).

5. Thread each bolt into the solenoid's threaded holes underneath. Tighten with
   your fingers, then snug with a screwdriver. The solenoid now hangs below the
   arm with its plunger pointing straight down.

6. Stick a thin rubber bumper pad (~1–2mm thick) onto the plunger tip. Clean the
   tip with rubbing alcohol first so the adhesive sticks. Then add a tiny drop
   of superglue around the edge — the bumper will take repeated impacts, so the
   glue keeps it from popping off over time.

**Build the bracket:**

7. **Attach the post to the base board.** Stand the 7.5" post vertically near
   one end of the base board. Hold a corner brace in the inside corner where
   the post meets the board. Mark the holes with a pencil, drill small pilot
   holes (this prevents the wood from splitting), then drive the corner brace
   screws. Repeat with a second corner brace on the other side of the post.

   Push the post sideways — it should feel solid and not wobble.

8. **Find the right arm height.** Stand the SSSCat can upright on the base board
   next to the post. Hold the arm + solenoid assembly against the post and
   slide it up or down until the rubber bumper on the plunger tip hovers about
   **5mm** (roughly the width of a pencil) above the can's valve stem.
   Mark this position on the post with a pencil.

   > **Why 5mm?** The bumper pad adds ~1–2mm, so the actual air gap is ~3–4mm.
   > The solenoid has 10mm of total stroke — plenty to cross the gap and press
   > the valve (which only needs ~1–2mm of push).

9. **Attach the arm to the post.** Hold the arm at your pencil mark. Place the
   third corner brace in the inside corner where the arm meets the post. Drill
   pilot holes, then drive the screws. The arm should stick out horizontally
   over where the can sits.

   Push down on the end of the arm — it should feel rigid. If it flexes, add a
   second corner brace on the opposite side of the joint.

10. **Clamp the can.** Stand the SSSCat can upright on the base board, directly
    below the solenoid. Look straight down from above to confirm the bumper is
    centered over the valve stem.

    Open the hose clamp, loop it around the can, and snug it at roughly
    mid-height on the can. Then drive a short wood screw through the hose
    clamp's band (the flat metal strip) into the base board — this pins the
    clamp so the can can't slide sideways.

    > **Do not over-tighten the hose clamp.** The can is thin-walled and
    > pressurized. Tighten until the can feels snug but the clamp isn't
    > visibly squeezing the can inward. Hand-tight on the worm screw is enough.

11. **Route the solenoid wires.** Run the two wires from the solenoid down along
    the post toward the base board. Secure them every few inches with a zip-tie
    or cable clip around the post. Leave a small loop of slack at the top so
    the wires don't tug when the solenoid fires. Connect to the relay and
    flyback diode as described in the [Wiring Diagram](#wiring-diagram) above.

12. **Add rubber feet.** Stick rubber or felt furniture pads on the four corners
    of the base board's underside. This protects your counter from scratches
    and keeps the whole assembly from sliding.

    > **Tip:** If your cat is strong enough to knock the bracket over, clamp the
    > base board to the counter edge with a small C-clamp, or stick heavy-duty
    > mounting tape to the underside.

13. **Test.** Upload the Hardware Test sketch (see [Step 3](#3-test-your-wiring-hardware-test)
    in Software Setup). You should hear a click and feel a puff of air from the
    nozzle. If no spray, reduce the gap by loosening the arm's corner brace and
    sliding the arm down slightly. If the plunger hits off-center, loosen the
    hose clamp screw and reposition the can.

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
| Solenoid clicks but can doesn't spray | Plunger not centered on valve — look from above and re-align. Or gap too large — loosen arm brace and slide down |
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
