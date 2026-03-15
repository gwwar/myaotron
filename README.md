# 🐱 myaotron

An automated cat-on-counter detection and deterrent system. Uses an AI camera to spot when your cat jumps on the kitchen counter and gives them a gentle, harmless puff of air to discourage the behavior — no human intervention needed.

**No prior electronics experience required.** This guide walks you through every step from buying parts to a working system.

## How It Works

A small AI camera ([HUSKYLENS 2](https://wiki.dfrobot.com/sen0638/docs/22636/)) watches your counter. It can recognize cats, surfaces, and people using built-in object recognition. When it sees a cat on the counter, an Arduino microcontroller opens a solenoid valve that releases a short puff of air — startling but completely harmless.

The system supports two detection modes:

| Mode | How It Works | When to Use |
|------|-------------|-------------|
| **MODE_DUAL_OVERLAP** (default) | Triggers only when the camera sees a cat *and* a counter surface overlapping | Recommended — fewest false alarms |
| **MODE_CAMERA_ONLY** | Triggers whenever any cat appears in the camera frame | Use if the camera points *only* at the counter |

## Shopping List

Below is everything you need, with links to trusted stores. The "core" items are required; the "pneumatic" items are for the recommended air-puff system.

### Core Electronics

| # | Item | Where to Buy | ~Price |
|---|------|-------------|--------|
| 1 | **HUSKYLENS 2** (SEN0638) — AI camera | [DFRobot](https://www.dfrobot.com/product-2995.html) · [DigiKey](https://www.digikey.com/en/products/detail/dfrobot/SEN0638/28531133) | $75–85 |
| 2 | **Arduino Uno R3** — main controller board | [Arduino Store](https://store.arduino.cc/products/arduino-uno-rev3) · [DFRobot (DFRduino)](https://www.dfrobot.com/product-838.html) | $25–28 |
| 3 | **Gravity IO Expansion Shield V7.1** — plugs onto the Arduino and gives you easy screw-terminal pin access (no soldering needed) | [DFRobot](https://www.dfrobot.com/product-1009.html) · [Amazon](https://www.amazon.com/DFROBOT-Gravity-Expansion-Shield-Arduino/dp/B00G3IKO68) | $5 |
| 4 | **4-pin I2C Gravity cable** — connects HUSKYLENS 2 to the Arduino (often included with HUSKYLENS 2) | [DFRobot](https://www.dfrobot.com/product-1009.html) | — |
| 5 | **USB-C cable** — provides separate power to HUSKYLENS 2 (a phone charger cable works) | Any store | $5 |
| 6 | **USB-B cable** — connects Arduino to your computer for uploading code | Any store (often included with Arduino) | $5 |
| 7 | **2-channel 5V relay module** — switches the solenoid valve and air pump on/off | [Amazon (search "2 channel 5V relay module Arduino")](https://www.amazon.com/s?k=2+channel+5v+relay+module+arduino) · [DFRobot (single channel)](https://www.dfrobot.com/product-64.html) | $4–8 |
| 8 | **Jumper wires (male-to-female)** — for connecting the relay module to the Arduino | [Adafruit](https://www.adafruit.com/product/1954) · [Amazon](https://www.amazon.com/s?k=jumper+wires+male+to+female) | $4 |

### Pneumatic System (Air Puff)

| # | Item | Where to Buy | ~Price |
|---|------|-------------|--------|
| 9 | **12V normally-closed solenoid valve** (1/4" NPT, direct-acting, rated for air) — releases air when activated. Get a **pneumatic** valve, not a water valve. | [U.S. Solid (pneumatic)](https://ussolid.com/products/u-s-solid-1-4-npt-pneumatic-electric-solenoid-valve-12-v-dc-2-way-2-position-normally-closed-html) · [Amazon (search "12V pneumatic solenoid valve 1/4 direct acting")](https://www.amazon.com/s?k=12V+pneumatic+solenoid+valve+1%2F4+direct+acting) | $10–18 |
| 10 | **12V mini diaphragm air pump** — fills the reservoir with pressurized air. Must be 12V to share the power supply. | [SparkFun](https://www.sparkfun.com/vacuum-pump-12v.html) · [Amazon (search "12V mini diaphragm air pump")](https://www.amazon.com/s?k=12V+mini+diaphragm+air+pump) | $10–15 |
| 11 | **Air reservoir** (~0.5L) — holds pressurized air between sprays. A small sealed PVC pipe cap, plastic bottle with fittings, or a [small pneumatic tank](https://www.amazon.com/s?k=small+air+reservoir+tank+pneumatic) works | Hardware store / Amazon | $5–10 |
| 12 | **One-way check valve** (1/4") — prevents air from flowing backward out of the reservoir | [Amazon (search "1/4 inline check valve air")](https://www.amazon.com/s?k=1%2F4+inline+check+valve+air) | $3–5 |
| 13 | **Adjustable pressure relief valve** (≤10 PSI) — **critical safety part** that prevents over-pressurization | [Amazon (search "adjustable pressure relief valve 10 PSI")](https://www.amazon.com/s?k=adjustable+pressure+relief+valve+10+psi) | $5–10 |
| 14 | **1/4" ID silicone or polyurethane tubing** (~3 ft) — connects all the pneumatic components. Make sure the inner diameter matches your fittings (1/4" ID = ~6mm). | [Amazon (search "1/4 ID silicone tubing")](https://www.amazon.com/s?k=1%2F4+ID+silicone+tubing) | $5–8 |
| 15 | **Hose barb fittings + hose clamps** — secure the tubing to each component | Hardware store / Amazon | $5 |
| 16 | **Wide-bore nozzle or aquarium air stone** — diffuses the air into a gentle puff (not a focused jet) | [Amazon (search "aquarium air stone")](https://www.amazon.com/s?k=aquarium+air+stone) | $2–5 |

### Power

| # | Item | Where to Buy | ~Price |
|---|------|-------------|--------|
| 17 | **12V DC power supply** (at least 2A) — powers the solenoid and pump. A 1A supply is NOT enough; the solenoid (~400mA) and pump (~1A) run simultaneously. | [Amazon (search "12V 2A DC power supply barrel jack")](https://www.amazon.com/s?k=12V+2A+DC+power+supply+barrel+jack+UL) | $8–12 |
| 18 | **DC barrel jack to screw terminal adapter** — makes it easy to connect the 12V supply to wires without soldering | [Adafruit (PID 368)](https://www.adafruit.com/product/368) · [Amazon](https://www.amazon.com/s?k=DC+barrel+jack+screw+terminal) | $3 |

**Estimated total: $90–140** (depending on what you already have).

> **Tip for beginners:** If you want to test the electronics before building the pneumatic system, you can start with just items 1–8. The relay will click audibly when a cat is detected, confirming everything works before you add air.

> **How big is it?** The electronics (Arduino + shield + relay) fit in a box about the size of a deck of cards. The pneumatic parts (pump, reservoir, solenoid, valves) add roughly the volume of a small water bottle. The whole system can sit behind the counter backsplash, on a shelf, or in a small project enclosure beside the counter — only the camera and nozzle need to face the counter surface.

## Wiring — Step by Step

> **What is wiring?** You're connecting components together with cables so they can communicate and share power. No soldering is needed for this project — everything uses plug-in cables or screw terminals.

### Step 1: Attach the Expansion Shield to the Arduino

The Gravity IO Expansion Shield plugs directly onto the top of the Arduino Uno. Line up all the pins and press down firmly until it's seated. This gives you labeled screw terminals for every pin, so you won't need to count tiny pin numbers.

### Step 2: Connect HUSKYLENS 2 to the Arduino

The HUSKYLENS 2 communicates with the Arduino using a protocol called **I2C** (a 4-wire digital connection).

1. Plug one end of the **4-pin I2C Gravity cable** into the I2C port on the back of HUSKYLENS 2
2. Connect the other end to the expansion shield:
   - **SDA** wire → **A4** pin on the shield
   - **SCL** wire → **A5** pin on the shield
   - **GND** wire → any **GND** pin on the shield
   - *(The VCC wire can be left unconnected — see power note below)*
3. Plug a **USB-C cable** from HUSKYLENS 2 to a USB power source (phone charger, USB battery pack, or powered USB hub)

> **Why separate power?** The Arduino can't supply enough power through its pins for the HUSKYLENS 2's AI processor. The USB-C cable provides dedicated power.

### Step 3: Connect the Solenoid Valve Relay (Channel 1)

The relay module is an electrically-controlled switch. When the Arduino sends a signal, the relay closes a circuit that turns the solenoid valve on.

1. Using a **jumper wire**, connect relay module **IN1** → shield pin **D7**
2. Connect relay module **VCC** → shield **5V**
3. Connect relay module **GND** → shield **GND**

### Step 4: Connect the Air Pump Relay (Channel 2) — Optional

If you're using an air pump (recommended), connect the second relay channel:

1. Connect relay module **IN2** → shield pin **D8**
2. *(VCC and GND are shared with Channel 1 — already connected)*

### Step 5: Wire the 12V Power Circuit

The solenoid and pump need 12V, which comes from the external power supply — NOT from the Arduino.

```
                    ┌─────────────────────────────────────┐
                    │         12V DC Power Supply          │
                    │         (barrel jack adapter)        │
                    └──────┬─────────────────┬────────────┘
                      12V (+)              GND (-)
                        │                    │
            ┌───────────┴──────────┐         │
            │                      │         │
    ┌───────┴───────┐     ┌───────┴───────┐  │
    │  Relay Ch 1   │     │  Relay Ch 2   │  │
    │    COM pin    │     │    COM pin    │  │
    └───────┬───────┘     └───────┬───────┘  │
            │                      │         │
    ┌───────┴───────┐     ┌───────┴───────┐  │
    │  Relay Ch 1   │     │  Relay Ch 2   │  │
    │    NO pin     │     │    NO pin     │  │
    └───────┬───────┘     └───────┬───────┘  │
            │                      │         │
     Solenoid (+)           Pump (+)         │
            │                      │         │
     Solenoid (-)───────── Pump (-)──────────┘
```

**What this means in plain English:**
1. Strip a small section of insulation from two wires (or use screw terminals)
2. Connect the **12V (+)** from the power supply to the **COM** (common) terminal on each relay channel
3. Connect the **NO** (normally open) terminal on relay channel 1 to the **positive (+)** wire of the solenoid valve
4. Connect the **NO** terminal on relay channel 2 to the **positive (+)** wire of the air pump
5. Connect the **negative (-)** wires of both the solenoid and pump back to the **GND (-)** of the power supply

> **What are COM and NO?** Each relay has three screw terminals:
> - **COM** (common) — the input wire goes here
> - **NO** (normally open) — the output when the relay is activated
> - **NC** (normally closed) — the output when the relay is *not* activated (we don't use this)

### Step 6: Assemble the Pneumatic Path

Connect the air components with tubing in this exact order:

```
[Air Pump outlet] → [Check Valve] → [Reservoir] → [Solenoid Valve] → [Nozzle]
                                         │
                                  [Pressure Relief Valve]
                                  (set to ≤10 PSI)
```

1. Connect tubing from the **pump's air outlet** to the **inlet side of the check valve** (the arrow on the check valve should point *away* from the pump)
2. Connect the **outlet side of the check valve** to the **reservoir inlet**
3. Attach the **pressure relief valve** to the reservoir (via a T-fitting or second port) — set it to **10 PSI or less**
4. Connect the **reservoir outlet** to the **solenoid valve inlet**
5. Connect the **solenoid valve outlet** to the **nozzle** (an aquarium air stone works great)
6. Secure all connections with **hose clamps** to prevent air leaks

> **⚠️ Safety notes:**
> - The **check valve** prevents air from flowing backward into the pump when it's off
> - The **pressure relief valve** is your hardware safety limit — it physically vents air if pressure exceeds your set limit, so the reservoir can never over-pressurize even if the software malfunctions
> - **10 PSI is plenty** to startle a cat. Higher pressure is unnecessary and could be harmful
> - Use a **wide-bore nozzle** (like an air stone) to produce a broad, gentle puff — never a focused jet
> - Aim the nozzle at the counter surface, **not directly at where the cat's face would be**

### Step 7: Power Everything On

1. Plug the **Arduino** into your computer via USB (this also powers it)
2. Plug the **HUSKYLENS 2** USB-C cable into a power source
3. Plug the **12V power supply** into the wall

The Arduino's onboard LED should light up, and HUSKYLENS 2's screen should turn on.

## Software Setup

### 1. Install Arduino IDE

If you don't already have it:

1. Go to [arduino.cc/en/software](https://www.arduino.cc/en/software)
2. Download Arduino IDE for your operating system
3. Install and open it

### 2. Install the HUSKYLENS 2 Library

The Arduino needs a software library to talk to HUSKYLENS 2:

1. Go to [github.com/DFRobot/DFRobot_HuskylensV2](https://github.com/DFRobot/DFRobot_HuskylensV2)
2. Click the green **Code** button → **Download ZIP**
3. In Arduino IDE: **Sketch → Include Library → Add .ZIP Library...**
4. Select the ZIP file you downloaded and click **Open**

### 3. Train HUSKYLENS 2 to Recognize Your Cat and Counter

Before uploading the code, you need to teach HUSKYLENS 2 what to look for. This is done directly on the device using its touchscreen:

1. Power on HUSKYLENS 2 (connect the USB-C cable)
2. On the screen, navigate to **Object Recognition** mode
3. **Learn your cat (ID 1):**
   - Point the camera at your cat from the angle the camera will be mounted
   - Press **Button-A** (the function button on the top) to save — this becomes **ID 1**
   - Try to capture your cat from a few angles for better recognition
4. **Learn your counter surface (ID 2):**
   - Point the camera at your kitchen counter
   - Press **Button-A** to save — this becomes **ID 2**
5. **(Optional) Learn a person (ID 3):**
   - Point the camera at yourself standing at the counter
   - Press **Button-A** to save — this becomes **ID 3**
   - This enables "human exclusion" so the system won't spray when you're at the counter
6. **(Optional)** Go to Settings → **Export Model** to save your training. This way you won't lose it if the device resets.

### 4. Upload the Code

1. Open `myaotron/myaotron.ino` in Arduino IDE
2. In the **Tools** menu:
   - Set **Board** to "Arduino Uno" (or your board)
   - Set **Port** to the USB port your Arduino is plugged into (it usually shows up as something like `/dev/cu.usbmodem14101` on Mac or `COM3` on Windows)
3. Click the **Upload** button (→ arrow icon)
4. Wait for "Done uploading" to appear

### 5. Test It

1. In Arduino IDE, open **Tools → Serial Monitor**
2. Set the baud rate to **9600** (dropdown in the bottom-right)
3. You should see:
   ```
   myaotron starting...
   Detection mode: 2
   Debounce frames: 3
   Person exclusion: ON
   Watchdog: ON
   Pump mode: 0
   HUSKYLENS 2 connected. Object Recognition active.
   Ready — watching for cats on counter...
   ```
4. Point HUSKYLENS 2 at your cat on the counter — you should see detection messages and hear the relay click!

### 6. Configure (Optional)

All settings are in `myaotron/config.h`. Open it in Arduino IDE, change values, and re-upload. Key settings:

| Setting | What It Does | Default |
|---------|-------------|---------|
| `DETECTION_MODE` | `MODE_DUAL_OVERLAP` (cat + counter overlap) or `MODE_CAMERA_ONLY` (any cat in frame) | `MODE_DUAL_OVERLAP` |
| `MIN_CONFIDENCE` | How sure the camera must be (0–100). Higher = fewer false positives but might miss real cats | `30` |
| `DEBOUNCE_FRAMES` | How many consecutive frames must detect a cat before spraying. Higher = slower but more reliable | `3` |
| `OVERLAP_THRESHOLD` | How much the cat must overlap the counter (0.0–1.0) | `0.3` |
| `PERSON_EXCLUSION_ENABLED` | `1` to suppress spray when a person is at the counter | `1` |
| `DETERRENT_PIN` | Arduino pin connected to the solenoid relay | `7` |
| `DETERRENT_ACTIVE_LOW` | Set to `1` if your relay board's LED turns on when the input pin is LOW | `0` |
| `SPRAY_DURATION_MS` | How long the solenoid stays open per spray (milliseconds) | `500` |
| `SPRAY_COOLDOWN_MS` | Minimum wait between sprays (milliseconds) | `5000` |
| `PUMP_MODE` | `PUMP_MODE_NONE` (no pump), `PUMP_MODE_ON_DEMAND` (refill after spray), `PUMP_MODE_CONTINUOUS` (always running) | `PUMP_MODE_NONE` |
| `PUMP_PIN` | Arduino pin connected to the pump relay | `8` |
| `PUMP_ACTIVE_LOW` | Set to `1` if your pump relay is active-low | `0` |
| `PUMP_PRE_CHARGE_MS` | How long the pump runs at boot to fill the reservoir (on-demand mode) | `3000` |
| `PUMP_REFILL_MS` | How long the pump runs after each spray to refill (on-demand mode) | `2000` |
| `STATUS_LED_PIN` | LED for visual status. Use `LED_BUILTIN` for the Arduino's onboard LED, or `-1` to disable | `LED_BUILTIN` |
| `LOOP_DELAY_MS` | Time between detection checks. Lower = faster response, higher CPU use | `200` |
| `WATCHDOG_ENABLED` | Auto-resets Arduino if the program freezes. Leave on for unattended use | `1` |
| `WATCHDOG_TIMEOUT_S` | Watchdog timeout in seconds (ESP32 only, AVR is fixed at 8s) | `10` |
| `RECONNECT_INTERVAL_MS` | How often to retry if HUSKYLENS 2 disconnects | `3000` |
| `DEBUG_SERIAL` | `1` to print detection info to Serial Monitor, `0` for silent | `1` |
| `SERIAL_BAUD` | Serial communication speed | `9600` |

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
  2. Check horizontal overlap ≥ OVERLAP_THRESHOLD
  3. Check cat's bottom edge is within surface bounds
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

Status LED (the Arduino's built-in LED):
  Off          = idle / cooldown (nothing happening)
  Slow blink   = debouncing (cat detected, confirming...)
  Solid on     = spraying
  Medium blink = refilling reservoir
  Fast blink   = error (HUSKYLENS 2 disconnected)
```

## Testing

### Desktop Unit Tests

The detection geometry logic is extracted into a standalone file (`detection_logic.h`) that can be tested on your computer without any Arduino hardware:

```bash
cd test && make
```

This compiles and runs 19 automated tests covering bounding box overlap, vertical positioning, and edge cases.

### Hardware Integration Test

Before deploying, upload `test_hardware/test_hardware.ino` to verify all your wiring is correct — no cat needed:

1. Open `test_hardware/test_hardware.ino` in Arduino IDE and upload
2. Open Serial Monitor (9600 baud)
3. The sketch cycles through:
   - **LED test** — blinks in each status pattern (verify visually)
   - **Deterrent relay test** — fires two brief pulses (you should hear clicks)
   - **Pump relay test** — fires a brief pulse if pump is enabled (listen for motor)
   - **HUSKYLENS test** — connects via I2C and reads one frame
4. Each subsystem reports **PASS** or **FAIL** on Serial

## Troubleshooting

| Problem | What to Check |
|---------|--------------|
| "HUSKYLENS 2 not found" | Is the I2C cable plugged in correctly (SDA→A4, SCL→A5)? Is the USB-C power cable connected to HUSKYLENS 2? |
| Cat detected but no spray | Is the relay wired to pin D7? Open Serial Monitor — do you see detection messages? Check the 12V power supply is on |
| Too many false positives | Increase `MIN_CONFIDENCE` (try `50`), `OVERLAP_THRESHOLD` (try `0.5`), or `DEBOUNCE_FRAMES` (try `5`) in config.h |
| Sprays when cat is on the floor | Make sure the counter surface is learned as ID 2. Try angling the camera more steeply downward |
| Sprays when I'm at the counter | Learn "person" as ID 3 and make sure `PERSON_EXCLUSION_ENABLED` is `1` in config.h |
| Counter not detected reliably | Lower the Detection Threshold in HUSKYLENS 2's on-device settings. Re-learn the surface from the camera's mounted position |
| LED blinking fast | HUSKYLENS 2 has disconnected — check I2C cable and USB-C power |
| Relay clicks but no air comes out | Is your relay active-low? Set `DETERRENT_ACTIVE_LOW` to `1`. Also check that the 12V supply is connected and the solenoid is wired to the NO terminal |
| Pump runs but no air puff | The check valve may be installed backward (arrow should point away from pump). Check tubing for kinks |
| Pump never stops running | Is `PUMP_MODE` set to `PUMP_MODE_CONTINUOUS` when you intended on-demand? Check `PUMP_REFILL_MS` value |
| Weak air puff | Increase `PUMP_PRE_CHARGE_MS` for a longer initial charge. Check all tubing connections for air leaks |
| Nothing happens at all | Is the Arduino powered (USB cable)? Is the correct board and port selected in Arduino IDE? Try re-uploading |

## Camera Placement Tips

- Mount the camera **2–3 feet above** the counter, angled **downward at ~45°**
- The camera should see **both the counter surface and the space above it** (where a cat would be)
- Avoid pointing the camera at windows — bright backlighting confuses object detection
- If using MODE_DUAL_OVERLAP, make sure the counter surface is large enough in the frame for reliable detection
- **Test first:** Open HUSKYLENS 2's live view and walk your cat near the counter to see how detection looks before mounting permanently

## License

MIT
