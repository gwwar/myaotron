#ifndef MYAOTRON_CONFIG_H
#define MYAOTRON_CONFIG_H

// ─── Detection Mode ──────────────────────────────────────────────
// DETECTION_MODE 1: Camera-only (approach 1)
//   Triggers on ANY cat detection in frame. Mount camera so it only
//   sees the counter surface.
//
// DETECTION_MODE 3: Dual-object overlap (approach 3)
//   Triggers only when a cat bounding box overlaps with a detected
//   surface (dining table). More robust against false positives.
#define DETECTION_MODE 3

// ─── HUSKYLENS Object IDs ────────────────────────────────────────
// Learn "cat" as ID 1 and "dining table" as ID 2 on your HUSKYLENS 2.
// See README.md for setup instructions.
#define CAT_ID 1
#define SURFACE_ID 2

// ─── Confidence Threshold ────────────────────────────────────────
// Minimum confidence (0–100) to consider a detection valid.
#define MIN_CONFIDENCE 30

// ─── Bounding Box Overlap ────────────────────────────────────────
// Minimum horizontal overlap ratio (0.0–1.0) between the cat bbox
// and the surface bbox to consider the cat "on" the surface.
#define OVERLAP_THRESHOLD 0.3

// ─── Deterrent Output ────────────────────────────────────────────
// Digital pin connected to relay/MOSFET controlling the solenoid
// valve or air pump.
#define DETERRENT_PIN 7

// Duration in milliseconds for each spray burst.
#define SPRAY_DURATION_MS 500

// Cooldown in milliseconds between consecutive sprays to avoid
// overwhelming the cat (and your solenoid).
#define SPRAY_COOLDOWN_MS 5000

// ─── Serial Debug ────────────────────────────────────────────────
#define DEBUG_SERIAL 1
#define SERIAL_BAUD 9600

#endif // MYAOTRON_CONFIG_H
