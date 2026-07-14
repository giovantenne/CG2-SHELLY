#include <Arduino.h>
#include <unity.h>

#include "app_state.h"
#include "app_store.h"

void test_setPowerReading_updates_fields() {
  shellyUpdatedAt = "old";
  lastShellyUpdatedAt = "";
  currentPowerWatts = 0.0f;
  phaseAPowerWatts = 0.0f;
  phaseBPowerWatts = 0.0f;
  phaseCPowerWatts = 0.0f;
  hasPowerReading = false;
  shellyOnline = false;
  shellyDataUpdated = false;

  setPowerReading(1234.5f, 100.0f, 200.0f, 934.5f, String("new"), true);

  TEST_ASSERT_EQUAL_STRING("old", lastShellyUpdatedAt.c_str());
  TEST_ASSERT_EQUAL_STRING("new", shellyUpdatedAt.c_str());
  TEST_ASSERT_EQUAL_FLOAT(1234.5f, currentPowerWatts);
  TEST_ASSERT_EQUAL_FLOAT(100.0f, phaseAPowerWatts);
  TEST_ASSERT_EQUAL_FLOAT(200.0f, phaseBPowerWatts);
  TEST_ASSERT_EQUAL_FLOAT(934.5f, phaseCPowerWatts);
  TEST_ASSERT_TRUE(hasPowerReading);
  TEST_ASSERT_TRUE(shellyOnline);
  TEST_ASSERT_TRUE(shellyDataUpdated);
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_setPowerReading_updates_fields);
  UNITY_END();
}

void loop() {}
