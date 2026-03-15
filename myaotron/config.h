#ifndef MYAOTRON_CONFIG_H
#define MYAOTRON_CONFIG_H

// ─── Detection Mode ──────────────────────────────────────────────
// MODE_CAMERA_ONLY: Triggers on ANY cat detection in frame. Mount
//   camera so it only sees the counter surface.
//
// MODE_DUAL_OVERLAP: Triggers only when a cat bounding box overlaps
//   with a detected surface (dining table). More robust against
//   false positives.
#define MODE_CAMERA_ONLY  1
#define MODE_DUAL_OVERLAP 2

#define DETECTION_MODE MODE_DUAL_OVERLAP

// ─── HUSKYLENS Object IDs ────────────────────────────────────────
// Learn "cat" as ID 1 and "dining table" as ID 2 on your HUSKYLENS 2.
// Optionally learn "person" as ID 3 to enable human exclusion.
// See README.md for setup instructions.
#define CAT_ID 1
#define SURFACE_ID 2
#define PERSON_ID 3

// ─── Confidence Threshold ────────────────────────────────────────
// Minimum confidence (0–100) to consider a detection valid.
// Detections below this are ignored entirely.
#define MIN_CONFIDENCE 30

// ─── Detection Debounce ──────────────────────────────────────────
// Number of consecutive positive detection frames required before
// triggering the deterrent. Prevents single-frame false positives.
// At 200ms loop delay, 3 frames ≈ 600ms confirmation window.
#define DEBOUNCE_FRAMES 3

// ─── Bounding Box Overlap ────────────────────────────────────────
// Minimum horizontal overlap ratio (0.0–1.0) between the cat bbox
// and the surface bbox to consider the cat "on" the surface.
#define OVERLAP_THRESHOLD 0.3

// ─── Person Exclusion ────────────────────────────────────────────
// When enabled, suppresses the deterrent if a person is detected
// overlapping the counter surface. Prevents spraying humans.
// Requires "person" to be learned as PERSON_ID on HUSKYLENS 2.
#define PERSON_EXCLUSION_ENABLED 1

// ─── Deterrent Output ────────────────────────────────────────────
// Digital pin connected to relay/MOSFET controlling the solenoid
// valve or air pump.
#define DETERRENT_PIN 7

// Set to 1 if your relay module is active-LOW (common with
// opto-isolated relay boards). 0 for active-HIGH.
#define DETERRENT_ACTIVE_LOW 0

// Duration in milliseconds for each spray burst.
#define SPRAY_DURATION_MS 500

// Cooldown in milliseconds between consecutive sprays to avoid
// overwhelming the cat (and your solenoid).
#define SPRAY_COOLDOWN_MS 5000

// ─── Status LED ──────────────────────────────────────────────────
// Optional LED for visual status feedback when serial is unavailable.
// Set to -1 to disable. Use LED_BUILTIN for the onboard LED.
//   Off     = idle (no detection)
//   Slow blink = cat detected, debouncing
//   Solid   = spraying
//   Fast blink = HUSKYLENS error
#define STATUS_LED_PIN LED_BUILTIN

// ─── HUSKYLENS Reconnection ─────────────────────────────────────
// Interval in milliseconds between reconnection attempts if
// communication with HUSKYLENS 2 is lost.
#define RECONNECT_INTERVAL_MS 3000

// ─── Serial Debug ────────────────────────────────────────────────
#define DEBUG_SERIAL 1
#define SERIAL_BAUD 9600

// ─── Loop Timing ─────────────────────────────────────────────────
// Delay in milliseconds between each detection loop iteration.
// Lower = faster response, higher CPU/I2C load.
// At 200ms, debounce of 3 frames ≈ 600ms confirmation window.
#define LOOP_DELAY_MS 200

// ─── Watchdog Timer ──────────────────────────────────────────────
// Enables hardware watchdog to auto-reset the board if the main
// loop hangs (e.g. I2C bus lockup). Highly recommended for
// unattended deployment.
// AVR: fixed 8s timeout. ESP32: configurable via WATCHDOG_TIMEOUT_S.
#define WATCHDOG_ENABLED 1
#define WATCHDOG_TIMEOUT_S 10

#endif // MYAOTRON_CONFIG_H
