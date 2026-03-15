/**
 * myaotron hardware integration test
 *
 * Upload this sketch to verify wiring and component connectivity
 * WITHOUT needing an actual cat. Cycles through each subsystem:
 *
 *   1. Status LED — blinks in each pattern (idle, debounce, spray, error)
 *   2. Deterrent relay — fires a brief test pulse (push solenoid clicks)
 *   3. HUSKYLENS 2 — connects and reads one frame
 *
 * Open Serial Monitor at 9600 baud to see results.
 * Each test prints PASS or FAIL.
 */

#include <Wire.h>
#include "DFRobot_HuskylensV2.h"
#include "../myaotron/config.h"

HuskylensV2 huskylens;

int passed = 0;
int failed = 0;

void report(const char *name, bool ok) {
  if (ok) {
    Serial.print(F("  [PASS] "));
    passed++;
  } else {
    Serial.print(F("  [FAIL] "));
    failed++;
  }
  Serial.println(name);
}

// ─── Test 1: Status LED ──────────────────────────────────────────

void testStatusLed() {
  Serial.println(F("\n--- Test 1: Status LED ---"));

#if STATUS_LED_PIN >= 0
  pinMode(STATUS_LED_PIN, OUTPUT);

  Serial.println(F("  Idle (off) — 1s"));
  digitalWrite(STATUS_LED_PIN, LOW);
  delay(1000);

  Serial.println(F("  Debounce (slow blink) — 2s"));
  for (int i = 0; i < 4; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(250);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(250);
  }

  Serial.println(F("  Spraying (solid on) — 1s"));
  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(1000);

  Serial.println(F("  Error (fast blink) — 2s"));
  for (int i = 0; i < 10; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(100);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(100);
  }

  digitalWrite(STATUS_LED_PIN, LOW);
  report("LED cycled through all patterns (verify visually)", true);
#else
  report("STATUS_LED_PIN disabled (-1), skipping", true);
#endif
}

// ─── Test 2: Deterrent Relay ─────────────────────────────────────

void testDeterrent() {
  Serial.println(F("\n--- Test 2: Deterrent Relay ---"));

  pinMode(DETERRENT_PIN, OUTPUT);

  // Start in off state
#if DETERRENT_ACTIVE_LOW
  digitalWrite(DETERRENT_PIN, HIGH);
#else
  digitalWrite(DETERRENT_PIN, LOW);
#endif

  Serial.println(F("  Activating relay for 200ms..."));
  Serial.println(F("  (You should hear a click or see the relay LED)"));

#if DETERRENT_ACTIVE_LOW
  digitalWrite(DETERRENT_PIN, LOW);
  delay(200);
  digitalWrite(DETERRENT_PIN, HIGH);
#else
  digitalWrite(DETERRENT_PIN, HIGH);
  delay(200);
  digitalWrite(DETERRENT_PIN, LOW);
#endif

  delay(500);

  Serial.println(F("  Deactivated. Firing second pulse in 1s..."));
  delay(1000);

#if DETERRENT_ACTIVE_LOW
  digitalWrite(DETERRENT_PIN, LOW);
  delay(200);
  digitalWrite(DETERRENT_PIN, HIGH);
#else
  digitalWrite(DETERRENT_PIN, HIGH);
  delay(200);
  digitalWrite(DETERRENT_PIN, LOW);
#endif

  report("Relay pulsed twice (verify click/LED visually)", true);
  Serial.print(F("  Pin: D")); Serial.println(DETERRENT_PIN);
  Serial.print(F("  Active-low: ")); Serial.println(DETERRENT_ACTIVE_LOW);
}

// ─── Test 3: HUSKYLENS Connection ────────────────────────────────

void testHuskylens() {
  Serial.println(F("\n--- Test 3: HUSKYLENS 2 Connection ---"));

  Wire.begin();
  Serial.println(F("  Attempting I2C connection..."));

  bool connected = false;
  for (int attempt = 1; attempt <= 3; attempt++) {
    Serial.print(F("  Attempt ")); Serial.print(attempt);
    Serial.println(F("/3..."));
    if (huskylens.begin(Wire)) {
      connected = true;
      break;
    }
    delay(1000);
  }

  report("HUSKYLENS 2 I2C connection", connected);
  if (!connected) {
    Serial.println(F("  Check: SDA->A4, SCL->A5, USB-C power to HUSKYLENS"));
    return;
  }

  // Switch to Object Recognition and try to read a frame
  Serial.println(F("  Switching to Object Recognition..."));
  bool switched = huskylens.switchAlgorithm(ALGORITHM_OBJECT_RECOGNITION);
  report("Algorithm switch", switched);

  Serial.println(F("  Reading one frame..."));
  int8_t result = huskylens.getResult(ALGORITHM_OBJECT_RECOGNITION);
  report("Frame read (result >= 0)", result >= 0);

  if (huskylens.available(ALGORITHM_OBJECT_RECOGNITION)) {
    int16_t count = huskylens.getCachedResultNum(ALGORITHM_OBJECT_RECOGNITION);
    Serial.print(F("  Objects detected in frame: ")); Serial.println(count);
    report("Object detection working", count >= 0);
  } else {
    Serial.println(F("  No objects in frame (point camera at something)"));
    report("Frame readable (no objects is OK)", true);
  }
}

// ─── Main ────────────────────────────────────────────────────────

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);

  Serial.println(F("=== myaotron hardware integration test ==="));

  testStatusLed();
  testDeterrent();
  testHuskylens();

  Serial.println(F("\n========================================="));
  Serial.print(F("Results: "));
  Serial.print(passed); Serial.print(F(" passed, "));
  Serial.print(failed); Serial.println(F(" failed"));

  if (failed == 0) {
    Serial.println(F("All hardware checks passed!"));
  } else {
    Serial.println(F("Some checks failed — see details above."));
  }
  Serial.println(F("========================================="));
}

void loop() {
  // Nothing — test runs once in setup
}
