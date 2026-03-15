/**
 * myaotron — Cat-on-counter detection and deterrent system
 *
 * Uses HUSKYLENS 2 (SEN0638) Object Recognition to detect cats on
 * counters and trigger a spray deterrent (air puff or water).
 *
 * Detection modes (set in config.h):
 *   MODE_DUAL_OVERLAP: triggers when a "cat" bbox overlaps a "dining
 *     table" bbox (default, most robust).
 *   MODE_CAMERA_ONLY: triggers on any cat detection in frame. Mount
 *     camera aimed only at the counter surface.
 *
 * Hardware:
 *   - Arduino Uno/Mega or ESP32
 *   - HUSKYLENS 2 (I2C connection)
 *   - Relay/MOSFET on DETERRENT_PIN → solenoid valve (air release)
 *   - (Optional) Second relay on PUMP_PIN → 12V diaphragm air pump
 *
 * Setup:
 *   1. On HUSKYLENS 2, enter Object Recognition mode.
 *   2. Learn your cat as ID 1 and your counter surface as ID 2.
 *   3. Optionally learn "person" as ID 3 for human exclusion.
 *   4. Upload this sketch and wire the deterrent relay.
 */

#include <Wire.h>
#include "DFRobot_HuskylensV2.h"
#include "config.h"
#include "detection_logic.h"

// ─── Watchdog ────────────────────────────────────────────────────
#if WATCHDOG_ENABLED
  #ifdef ESP32
    #include <esp_task_wdt.h>
  #else
    #include <avr/wdt.h>
  #endif
#endif

static inline void watchdogInit() {
#if WATCHDOG_ENABLED
  #ifdef ESP32
    esp_task_wdt_init(WATCHDOG_TIMEOUT_S, true);
    esp_task_wdt_add(NULL);
  #else
    wdt_enable(WDTO_8S);
  #endif
#endif
}

static inline void watchdogReset() {
#if WATCHDOG_ENABLED
  #ifdef ESP32
    esp_task_wdt_reset();
  #else
    wdt_reset();
  #endif
#endif
}

HuskylensV2 huskylens;

// ─── State ───────────────────────────────────────────────────────

enum SystemState {
  STATE_IDLE,
  STATE_DEBOUNCING,
  STATE_SPRAYING,
  STATE_REFILLING,
  STATE_COOLDOWN,
  STATE_ERROR
};

SystemState state = STATE_IDLE;
uint8_t debounceCount = 0;
unsigned long sprayStartTime = 0;
unsigned long lastSprayEndTime = 0;
unsigned long lastReconnectAttempt = 0;
unsigned long refillStartTime = 0;
bool huskyConnected = false;
uint16_t sprayCount = 0;

// LED blink tracking
unsigned long lastLedToggle = 0;
bool ledState = false;

// ─── Deterrent helpers ───────────────────────────────────────────

void setDeterrent(bool active) {
#if DETERRENT_ACTIVE_LOW
  digitalWrite(DETERRENT_PIN, active ? LOW : HIGH);
#else
  digitalWrite(DETERRENT_PIN, active ? HIGH : LOW);
#endif
}

// ─── Pump helpers ────────────────────────────────────────────────

void setPump(bool active) {
#if PUMP_MODE != PUMP_MODE_NONE
  #if PUMP_ACTIVE_LOW
    digitalWrite(PUMP_PIN, active ? LOW : HIGH);
  #else
    digitalWrite(PUMP_PIN, active ? HIGH : LOW);
  #endif
#endif
}

// ─── Status LED ──────────────────────────────────────────────────

void updateStatusLed() {
#if STATUS_LED_PIN >= 0
  unsigned long now = millis();
  switch (state) {
    case STATE_IDLE:
      digitalWrite(STATUS_LED_PIN, LOW);
      break;
    case STATE_DEBOUNCING:
      // Slow blink: 500ms period
      if (now - lastLedToggle >= 250) {
        ledState = !ledState;
        digitalWrite(STATUS_LED_PIN, ledState);
        lastLedToggle = now;
      }
      break;
    case STATE_SPRAYING:
      digitalWrite(STATUS_LED_PIN, HIGH);
      break;
    case STATE_REFILLING:
      // Medium blink: 400ms period
      if (now - lastLedToggle >= 200) {
        ledState = !ledState;
        digitalWrite(STATUS_LED_PIN, ledState);
        lastLedToggle = now;
      }
      break;
    case STATE_COOLDOWN:
      digitalWrite(STATUS_LED_PIN, LOW);
      break;
    case STATE_ERROR:
      // Fast blink: 200ms period
      if (now - lastLedToggle >= 100) {
        ledState = !ledState;
        digitalWrite(STATUS_LED_PIN, ledState);
        lastLedToggle = now;
      }
      break;
  }
#endif
}

// ─── HUSKYLENS connection ────────────────────────────────────────

bool tryConnect() {
  if (huskylens.begin(Wire)) {
    huskylens.switchAlgorithm(ALGORITHM_OBJECT_RECOGNITION);
    huskyConnected = true;
    #if DEBUG_SERIAL
    Serial.println(F("HUSKYLENS 2 connected. Object Recognition active."));
    #endif
    return true;
  }
  return false;
}

// Returns false if HUSKYLENS is disconnected and sets error state.
bool ensureConnected() {
  if (huskyConnected) return true;

  unsigned long now = millis();
  if (now - lastReconnectAttempt < RECONNECT_INTERVAL_MS) return false;
  lastReconnectAttempt = now;

  #if DEBUG_SERIAL
  Serial.println(F("Attempting HUSKYLENS reconnect..."));
  #endif

  if (tryConnect()) {
    state = STATE_IDLE;
    return true;
  }

  state = STATE_ERROR;
  return false;
}

// ─── Bounding box helpers ────────────────────────────────────────
// BBox struct and geometry functions are in detection_logic.h

BBox resultToBBox(Result *r) {
  BBox box = {0, 0, 0, 0, 0, false};
  if (r != NULL) {
    box.x = RET_ITEM_NUM(r, Result, xCenter);
    box.y = RET_ITEM_NUM(r, Result, yCenter);
    box.w = RET_ITEM_NUM(r, Result, width);
    box.h = RET_ITEM_NUM(r, Result, height);
    box.confidence = RET_ITEM_NUM(r, Result, confidence);
    box.valid = (box.w > 0 && box.h > 0 && box.confidence >= MIN_CONFIDENCE);
  }
  return box;
}

BBox getBBoxForID(int16_t id) {
  Result *r = huskylens.getCachedResultByID(ALGORITHM_OBJECT_RECOGNITION, id);
  return resultToBBox(r);
}

// Check if a person overlaps the counter surface (for exclusion).
bool isPersonAtCounter() {
#if PERSON_EXCLUSION_ENABLED
  int16_t numPersons = huskylens.getCachedResultNumByID(ALGORITHM_OBJECT_RECOGNITION, PERSON_ID);
  if (numPersons <= 0) return false;

  BBox surfaceBox = getBBoxForID(SURFACE_ID);
  if (!surfaceBox.valid) return false;

  for (int16_t i = 0; i < numPersons; i++) {
    Result *r = huskylens.getCachedIndexResultByID(ALGORITHM_OBJECT_RECOGNITION, PERSON_ID, i);
    BBox personBox = resultToBBox(r);
    if (personBox.valid
        && horizontalOverlap(personBox, surfaceBox) >= OVERLAP_THRESHOLD
        && isVerticallyOnSurface(personBox, surfaceBox)) {
      #if DEBUG_SERIAL
      Serial.println(F("Person detected at counter — suppressing deterrent."));
      #endif
      return true;
    }
  }
#endif
  return false;
}

// ─── Detection logic ─────────────────────────────────────────────

// Check if any cat instance overlaps the surface.
bool anyCatOnSurface(BBox surfaceBox) {
  int16_t numCats = huskylens.getCachedResultNumByID(ALGORITHM_OBJECT_RECOGNITION, CAT_ID);

  for (int16_t i = 0; i < numCats; i++) {
    Result *r = huskylens.getCachedIndexResultByID(ALGORITHM_OBJECT_RECOGNITION, CAT_ID, i);
    BBox catBox = resultToBBox(r);
    if (!catBox.valid) continue;

    float hOverlap = horizontalOverlap(catBox, surfaceBox);
    bool onSurface = isVerticallyOnSurface(catBox, surfaceBox);

    #if DEBUG_SERIAL
    Serial.print(F("  Cat["));  Serial.print(i);
    Serial.print(F("] conf=")); Serial.print(catBox.confidence);
    Serial.print(F(" overlap=")); Serial.print(hOverlap);
    Serial.print(F(" onSurf=")); Serial.println(onSurface);
    #endif

    if (hOverlap >= OVERLAP_THRESHOLD && onSurface) {
      return true;
    }
  }
  return false;
}

bool detectCatOnCounter() {
  int8_t result = huskylens.getResult(ALGORITHM_OBJECT_RECOGNITION);
  if (result <= 0) {
    // Communication failure — mark disconnected
    huskyConnected = false;
    state = STATE_ERROR;
    #if DEBUG_SERIAL
    Serial.println(F("HUSKYLENS communication lost."));
    #endif
    return false;
  }

  if (!huskylens.available(ALGORITHM_OBJECT_RECOGNITION)) {
    return false;
  }

#if DETECTION_MODE == MODE_CAMERA_ONLY
  // Camera-only: Any cat in frame = cat on counter.
  int16_t numCats = huskylens.getCachedResultNumByID(ALGORITHM_OBJECT_RECOGNITION, CAT_ID);
  for (int16_t i = 0; i < numCats; i++) {
    Result *r = huskylens.getCachedIndexResultByID(ALGORITHM_OBJECT_RECOGNITION, CAT_ID, i);
    BBox catBox = resultToBBox(r);
    if (catBox.valid) {
      #if DEBUG_SERIAL
      Serial.print(F("[CAMERA] Cat detected, conf="));
      Serial.println(catBox.confidence);
      #endif
      return true;
    }
  }

#elif DETECTION_MODE == MODE_DUAL_OVERLAP
  // Dual-overlap: Cat + surface overlap detection.
  BBox surfaceBox = getBBoxForID(SURFACE_ID);

  if (!surfaceBox.valid) {
    #if DEBUG_SERIAL
    Serial.println(F("[OVERLAP] Surface not detected."));
    #endif
    return false;
  }

  if (isPersonAtCounter()) return false;

  if (anyCatOnSurface(surfaceBox)) {
    #if DEBUG_SERIAL
    Serial.println(F("[OVERLAP] Cat ON counter!"));
    #endif
    return true;
  }
#endif

  return false;
}

// ─── Main state machine ─────────────────────────────────────────

void setup() {
  // Safety first: ensure deterrent and pump are off before anything else
  pinMode(DETERRENT_PIN, OUTPUT);
  setDeterrent(false);
#if PUMP_MODE != PUMP_MODE_NONE
  pinMode(PUMP_PIN, OUTPUT);
  setPump(false);
#endif

#if STATUS_LED_PIN >= 0
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
#endif

  Serial.begin(SERIAL_BAUD);
  Wire.begin();

  Serial.println(F("myaotron starting..."));
  Serial.print(F("Detection mode: ")); Serial.println(DETECTION_MODE);
  Serial.print(F("Debounce frames: ")); Serial.println(DEBOUNCE_FRAMES);
  Serial.print(F("Person exclusion: "));
  Serial.println(PERSON_EXCLUSION_ENABLED ? F("ON") : F("OFF"));
  Serial.print(F("Watchdog: "));
  Serial.println(WATCHDOG_ENABLED ? F("ON") : F("OFF"));
  Serial.print(F("Pump mode: ")); Serial.println(PUMP_MODE);

  while (!tryConnect()) {
    Serial.println(F("HUSKYLENS 2 not found. Check wiring. Retrying..."));
    state = STATE_ERROR;
    updateStatusLed();
    delay(1000);
  }

  watchdogInit();

#if PUMP_MODE == PUMP_MODE_ON_DEMAND
  Serial.println(F("Pre-charging reservoir..."));
  setPump(true);
  delay(PUMP_PRE_CHARGE_MS);
  setPump(false);
  Serial.println(F("Pre-charge complete."));
#elif PUMP_MODE == PUMP_MODE_CONTINUOUS
  Serial.println(F("Starting continuous pump."));
  setPump(true);
#endif

  state = STATE_IDLE;
  Serial.println(F("Ready — watching for cats on counter..."));
}

void loop() {
  unsigned long now = millis();

  watchdogReset();
  updateStatusLed();

  switch (state) {
    case STATE_IDLE:
      if (!ensureConnected()) break;
      if (detectCatOnCounter()) {
        debounceCount = 1;
        if (DEBOUNCE_FRAMES <= 1) {
          // No debounce needed
          state = STATE_SPRAYING;
          sprayStartTime = now;
          setDeterrent(true);
          sprayCount++;
          #if DEBUG_SERIAL
          Serial.print(F(">>> SPRAY #")); Serial.print(sprayCount);
          Serial.println(F(" ACTIVATED <<<"));
          #endif
        } else {
          state = STATE_DEBOUNCING;
          #if DEBUG_SERIAL
          Serial.print(F("Debounce: 1/"));
          Serial.println(DEBOUNCE_FRAMES);
          #endif
        }
      }
      break;

    case STATE_DEBOUNCING:
      if (!ensureConnected()) break;
      if (detectCatOnCounter()) {
        debounceCount++;
        #if DEBUG_SERIAL
        Serial.print(F("Debounce: "));
        Serial.print(debounceCount);
        Serial.print(F("/"));
        Serial.println(DEBOUNCE_FRAMES);
        #endif
        if (debounceCount >= DEBOUNCE_FRAMES) {
          state = STATE_SPRAYING;
          sprayStartTime = now;
          setDeterrent(true);
          sprayCount++;
          #if DEBUG_SERIAL
          Serial.print(F(">>> SPRAY #")); Serial.print(sprayCount);
          Serial.println(F(" ACTIVATED <<<"));
          #endif
        }
      } else {
        // Detection lost during debounce — reset, unless HUSKYLENS disconnected
        debounceCount = 0;
        if (!huskyConnected) {
          // Error state was set by detectCatOnCounter(); don't overwrite it
          #if DEBUG_SERIAL
          Serial.println(F("Debounce aborted — HUSKYLENS disconnected."));
          #endif
        } else {
          state = STATE_IDLE;
          #if DEBUG_SERIAL
          Serial.println(F("Debounce reset — detection lost."));
          #endif
        }
      }
      break;

    case STATE_SPRAYING:
      if (now - sprayStartTime >= SPRAY_DURATION_MS) {
        setDeterrent(false);
#if PUMP_MODE == PUMP_MODE_ON_DEMAND
        // Refill the reservoir after spray
        setPump(true);
        refillStartTime = now;
        state = STATE_REFILLING;
        #if DEBUG_SERIAL
        Serial.println(F("Spray complete, refilling reservoir."));
        #endif
#else
        lastSprayEndTime = now;
        state = STATE_COOLDOWN;
        #if DEBUG_SERIAL
        Serial.println(F("Spray complete, entering cooldown."));
        #endif
#endif
      }
      // Non-blocking: loop continues running during spray
      break;

    case STATE_REFILLING:
      if (now - refillStartTime >= PUMP_REFILL_MS) {
        setPump(false);
        lastSprayEndTime = now;
        state = STATE_COOLDOWN;
        #if DEBUG_SERIAL
        Serial.println(F("Refill complete, entering cooldown."));
        #endif
      }
      break;

    case STATE_COOLDOWN:
      if (now - lastSprayEndTime >= SPRAY_COOLDOWN_MS) {
        state = STATE_IDLE;
        debounceCount = 0;
        #if DEBUG_SERIAL
        Serial.println(F("Cooldown complete, resuming detection."));
        #endif
      }
      break;

    case STATE_ERROR:
      setDeterrent(false);
#if PUMP_MODE == PUMP_MODE_ON_DEMAND
      setPump(false);
#elif PUMP_MODE == PUMP_MODE_CONTINUOUS
      setPump(false);
#endif
      ensureConnected();
#if PUMP_MODE == PUMP_MODE_CONTINUOUS
      // Restart pump when recovered
      if (huskyConnected) setPump(true);
#endif
      break;
  }

  delay(LOOP_DELAY_MS);
}
