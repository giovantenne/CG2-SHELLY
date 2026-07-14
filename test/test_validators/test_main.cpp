#include <Arduino.h>
#include <unity.h>

#include "config.h"

void test_isValidNumber() {
  TEST_ASSERT_TRUE(isValidNumber("123"));
  TEST_ASSERT_TRUE(isValidNumber("a1"));
  TEST_ASSERT_FALSE(isValidNumber("abc"));
  TEST_ASSERT_FALSE(isValidNumber(""));
}

void test_isValidEmail() {
  TEST_ASSERT_TRUE(isValidEmail("user@example.com"));
  TEST_ASSERT_TRUE(isValidEmail("u@e.co"));
  TEST_ASSERT_FALSE(isValidEmail("userexample.com"));
  TEST_ASSERT_FALSE(isValidEmail("user@ example.com"));
  TEST_ASSERT_FALSE(isValidEmail("user@ex"));
}

void test_isValidShellyDeviceId() {
  TEST_ASSERT_TRUE(isValidShellyDeviceId("a1b2c3d4e5f6"));
  TEST_ASSERT_TRUE(isValidShellyDeviceId("X12345"));
  TEST_ASSERT_FALSE(isValidShellyDeviceId(""));
  TEST_ASSERT_FALSE(isValidShellyDeviceId("a1b2c3d4e5f6!"));
}

void test_isValidPollingInterval() {
  TEST_ASSERT_TRUE(isValidPollingInterval("1"));
  TEST_ASSERT_TRUE(isValidPollingInterval("5"));
  TEST_ASSERT_TRUE(isValidPollingInterval("3600"));
  TEST_ASSERT_FALSE(isValidPollingInterval("0"));
  TEST_ASSERT_FALSE(isValidPollingInterval("3601"));
  TEST_ASSERT_FALSE(isValidPollingInterval("5s"));
  TEST_ASSERT_FALSE(isValidPollingInterval(""));
}

void test_isValidGaugeMaxKilowatts() {
  TEST_ASSERT_TRUE(isValidGaugeMaxKilowatts("1"));
  TEST_ASSERT_TRUE(isValidGaugeMaxKilowatts("6"));
  TEST_ASSERT_TRUE(isValidGaugeMaxKilowatts("100"));
  TEST_ASSERT_FALSE(isValidGaugeMaxKilowatts("0"));
  TEST_ASSERT_FALSE(isValidGaugeMaxKilowatts("101"));
  TEST_ASSERT_FALSE(isValidGaugeMaxKilowatts("6.5"));
  TEST_ASSERT_FALSE(isValidGaugeMaxKilowatts(""));
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_isValidNumber);
  RUN_TEST(test_isValidEmail);
  RUN_TEST(test_isValidShellyDeviceId);
  RUN_TEST(test_isValidPollingInterval);
  RUN_TEST(test_isValidGaugeMaxKilowatts);
  UNITY_END();
}

void loop() {}
