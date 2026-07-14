#include <Arduino.h>
#include <unity.h>

#include "api.h"
#include "app_state.h"

void test_switch_power_fallback_parser() {
  currentPowerWatts = 0.0f;

  bool ok = testApplyShellyStatusJson("{\"switch:0\":{\"apower\":42.5}}");

  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_FLOAT(42.5f, currentPowerWatts);
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_switch_power_fallback_parser);
  UNITY_END();
}

void loop() {}
