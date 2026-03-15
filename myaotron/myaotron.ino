/**
 * myaotron — Cat-on-counter detection and deterrent system
 *
 * Uses HUSKYLENS 2 (SEN0638) Object Recognition to detect cats on
 * counters and trigger a spray deterrent (air puff or water).
 *
 * Detection modes (set in config.h):
 *   Mode 3: Dual-object overlap — triggers when a "cat" bbox overlaps
 *           a "dining table" bbox (default, most robust).
 *   Mode 1: Camera-only — triggers on any cat detection in frame.
 *           Mount camera aimed only at the counter surface.
 *
 * Hardware:
 *   - Arduino Uno/Mega or ESP32
 *   - HUSKYLENS 2 (I2C connection)
 *   - Relay/MOSFET on DETERRENT_PIN → solenoid valve or air pump
 *
 * Setup:
 *   1. On HUSKYLENS 2, enter Object Recognition mode.
 *   2. Learn your cat as ID 1 and your counter surface as ID 2.
 *   3. Upload this sketch and wire the deterrent relay.
 */

#include <Wire.h>
#include "DFRobot_HuskylensV2.h"
#include "config.h"

HuskylensV2 huskylens;

unsigned long lastSprayTime = 0;

// ─── Bounding box helper ─────────────────────────────────────────

struct BBox {
  int16_t x, y, w, h;
  bool valid;
};

BBox getBBoxForID(int16_t id) {
  BBox box = {0, 0, 0, 0, false};
  Result *r = huskylens.getCachedResultByID(ALGORITHM_OBJECT_RECOGNITION, id);
  if (r != NULL) {
    box.x = RET_ITEM_NUM(r, Result, xCenter);
    box.y = RET_ITEM_NUM(r, Result, yCenter);
    box.w = RET_ITEM_NUM(r, Result, width);
    box.h = RET_ITEM_NUM(r, Result, height);
    box.valid = (box.x >= 0 && box.w > 0);
  }
  return box;
}

// Returns the horizontal overlap ratio of box A within box B.
// 1.0 = A is fully inside B horizontally; 0.0 = no overlap.
float horizontalOverlap(BBox a, BBox b) {
  int16_t aLeft  = a.x - a.w / 2;
  int16_t aRight = a.x + a.w / 2;
  int16_t bLeft  = b.x - b.w / 2;
  int16_t bRight = b.x + b.w / 2;

  int16_t overlapLeft  = max(aLeft, bLeft);
  int16_t overlapRight = min(aRight, bRight);
  int16_t overlapWidth = overlapRight - overlapLeft;

  if (overlapWidth <= 0 || a.w <= 0) return 0.0;
  return (float)overlapWidth / (float)a.w;
}

// Check if the cat bbox is vertically positioned on/above the surface.
// In camera coordinates, y=0 is the top of the frame, so "on the
// surface" means the cat's bottom edge is near the surface's top half.
bool isVerticallyOnSurface(BBox cat, BBox surface) {
  int16_t catBottom    = cat.y + cat.h / 2;
  int16_t surfaceTop   = surface.y - surface.h / 2;
  int16_t surfaceBot   = surface.y + surface.h / 2;

  return (catBottom >= surfaceTop && catBottom <= surfaceBot);
}

// ─── Detection logic ─────────────────────────────────────────────

bool detectCatOnCounter() {
  huskylens.getResult(ALGORITHM_OBJECT_RECOGNITION);

  if (!huskylens.available(ALGORITHM_OBJECT_RECOGNITION)) {
    return false;
  }

#if DETECTION_MODE == 1
  // Mode 1: Any cat detection in frame = cat on counter.
  // Relies on camera being aimed only at the counter.
  Result *cat = huskylens.getCachedResultByID(ALGORITHM_OBJECT_RECOGNITION, CAT_ID);
  if (cat != NULL) {
    #if DEBUG_SERIAL
    Serial.println(F("[MODE 1] Cat detected in frame!"));
    #endif
    return true;
  }

#elif DETECTION_MODE == 3
  // Mode 3: Cat + surface overlap detection.
  BBox catBox     = getBBoxForID(CAT_ID);
  BBox surfaceBox = getBBoxForID(SURFACE_ID);

  if (!catBox.valid) return false;

  if (!surfaceBox.valid) {
    // Surface not visible — fall through without triggering.
    // To fallback to mode 1 when surface is lost, uncomment below:
    // #if DEBUG_SERIAL
    // Serial.println(F("[MODE 3] Surface lost, falling back to mode 1"));
    // #endif
    // return true;
    return false;
  }

  float hOverlap = horizontalOverlap(catBox, surfaceBox);
  bool onSurface = isVerticallyOnSurface(catBox, surfaceBox);

  #if DEBUG_SERIAL
  Serial.print(F("[MODE 3] Cat: ("));
  Serial.print(catBox.x); Serial.print(F(",")); Serial.print(catBox.y);
  Serial.print(F(") ")); Serial.print(catBox.w); Serial.print(F("x")); Serial.print(catBox.h);
  Serial.print(F(" | Surface: ("));
  Serial.print(surfaceBox.x); Serial.print(F(",")); Serial.print(surfaceBox.y);
  Serial.print(F(") ")); Serial.print(surfaceBox.w); Serial.print(F("x")); Serial.println(surfaceBox.h);
  Serial.print(F("  hOverlap=")); Serial.print(hOverlap);
  Serial.print(F(" onSurface=")); Serial.println(onSurface);
  #endif

  if (hOverlap >= OVERLAP_THRESHOLD && onSurface) {
    #if DEBUG_SERIAL
    Serial.println(F("[MODE 3] Cat ON counter — triggering deterrent!"));
    #endif
    return true;
  }

#endif

  return false;
}

// ─── Deterrent control ───────────────────────────────────────────

void activateDeterrent() {
  unsigned long now = millis();
  if (now - lastSprayTime < SPRAY_COOLDOWN_MS) {
    #if DEBUG_SERIAL
    Serial.println(F("Deterrent on cooldown, skipping."));
    #endif
    return;
  }

  #if DEBUG_SERIAL
  Serial.println(F(">>> SPRAY ACTIVATED <<<"));
  #endif

  digitalWrite(DETERRENT_PIN, HIGH);
  delay(SPRAY_DURATION_MS);
  digitalWrite(DETERRENT_PIN, LOW);

  lastSprayTime = millis();
}

// ─── Setup & loop ────────────────────────────────────────────────

void setup() {
  Serial.begin(SERIAL_BAUD);
  Wire.begin();

  pinMode(DETERRENT_PIN, OUTPUT);
  digitalWrite(DETERRENT_PIN, LOW);

  Serial.println(F("myaotron starting..."));
  Serial.print(F("Detection mode: "));
  Serial.println(DETECTION_MODE);

  while (!huskylens.begin(Wire)) {
    Serial.println(F("HUSKYLENS 2 not found. Check wiring. Retrying..."));
    delay(1000);
  }

  huskylens.switchAlgorithm(ALGORITHM_OBJECT_RECOGNITION);
  Serial.println(F("HUSKYLENS 2 connected. Object Recognition active."));
  Serial.println(F("Ready — watching for cats on counter..."));
}

void loop() {
  if (detectCatOnCounter()) {
    activateDeterrent();
  }
  delay(200);
}
