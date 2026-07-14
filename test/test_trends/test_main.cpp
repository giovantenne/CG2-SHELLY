#include <Arduino.h>
#include <unity.h>

#include "api.h"
#include "app_state.h"

void test_em_total_power_parser() {
  currentPowerWatts = 0.0f;
  phaseAPowerWatts = 0.0f;
  phaseBPowerWatts = 0.0f;
  phaseCPowerWatts = 0.0f;

  bool ok = testApplyShellyStatusJson("{\"_updated\":\"2026-07-14 11:30:00\",\"em:0\":{\"a_act_power\":100.5,\"b_act_power\":200,\"c_act_power\":300,\"total_act_power\":600.5}}");

  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_FLOAT(600.5f, currentPowerWatts);
  TEST_ASSERT_EQUAL_FLOAT(100.5f, phaseAPowerWatts);
  TEST_ASSERT_EQUAL_FLOAT(200.0f, phaseBPowerWatts);
  TEST_ASSERT_EQUAL_FLOAT(300.0f, phaseCPowerWatts);
  TEST_ASSERT_EQUAL_STRING("2026-07-14 11:30:00", shellyUpdatedAt.c_str());
}

void test_em1_power_parser_sums_channels() {
  currentPowerWatts = 0.0f;
  phaseAPowerWatts = 0.0f;
  phaseBPowerWatts = 0.0f;
  phaseCPowerWatts = 0.0f;

  bool ok = testApplyShellyStatusJson("{\"em1:0\":{\"act_power\":800.25},\"em1:1\":{\"act_power\":1200.75}}");

  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_FLOAT(2001.0f, currentPowerWatts);
  TEST_ASSERT_EQUAL_FLOAT(800.25f, phaseAPowerWatts);
  TEST_ASSERT_EQUAL_FLOAT(1200.75f, phaseBPowerWatts);
  TEST_ASSERT_EQUAL_FLOAT(0.0f, phaseCPowerWatts);
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_em_total_power_parser);
  RUN_TEST(test_em1_power_parser_sums_channels);
  UNITY_END();
}

void loop() {}
