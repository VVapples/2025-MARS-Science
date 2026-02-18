#pragma once

#include <Arduino.h>

#ifndef SERIAL_ATTACH_TIMEOUT_MS
#define SERIAL_ATTACH_TIMEOUT_MS 15000UL
#endif

#ifndef SERIAL_PRE_TEST_DELAY_MS
#define SERIAL_PRE_TEST_DELAY_MS 8000UL
#endif

#ifndef SERIAL_POST_TEST_HOLD_MS
#define SERIAL_POST_TEST_HOLD_MS 5000UL
#endif

namespace TestHarness {

inline bool isHostListening() {
#if defined(ARDUINO_TEENSY41) || defined(CORE_TEENSY)
  return Serial && Serial.dtr();
#else
  return Serial;
#endif
}

inline void waitForSerialAttach(const char* tag) {
  Serial.begin(115200);

  const unsigned long start = millis();
  while (!isHostListening() && (millis() - start) < SERIAL_ATTACH_TIMEOUT_MS) {
    delay(50);
  }

  Serial.printf("\n[%s] Host detect phase done. Forced pre-test delay: %lu ms.\n",
                tag,
                (unsigned long)SERIAL_PRE_TEST_DELAY_MS);

  const unsigned long waitStart = millis();
  while ((millis() - waitStart) < SERIAL_PRE_TEST_DELAY_MS) {
    const unsigned long left = SERIAL_PRE_TEST_DELAY_MS - (millis() - waitStart);
    Serial.printf("[%s] Starting in %lu ms...\n", tag, left);
    delay(1000);
  }
}

inline void holdAfterTests(const char* tag) {
  Serial.printf("[%s] Tests complete. Holding serial for %lu ms.\n", tag, (unsigned long)SERIAL_POST_TEST_HOLD_MS);
  const unsigned long start = millis();
  while ((millis() - start) < SERIAL_POST_TEST_HOLD_MS) {
    delay(100);
  }
}

}  // namespace TestHarness
