#include <Arduino.h>
#include <unity.h>

#include <EEPROM.h>
#include "board.h"
#include "config_store.h"

// Local reader to mirror how we persisted
static String readStringFrom(int addr) {
  int len = EEPROM.read(addr) | (EEPROM.read(addr + 1) << 8);
  char buf[1700];
  int capped = len < 1699 ? len : 1699;
  for (int i = 0; i < capped; i++) {
    buf[i] = EEPROM.read(addr + 2 + i);
  }
  buf[capped] = '\0';
  return String(buf);
}

void test_configstore_persist_roundtrip() {
  EEPROM.begin(EEPROM_SIZE);
  configStoreSetBrightness(77);
  configStoreSetShellyCredentials("a1b2c3d4e5f6", "auth-key");
  configStoreSetPollingInterval(12);
  configStoreSetGaugeMaxKilowatts(15);
  configStorePersist();

  int addr = 0;
  TEST_ASSERT_EQUAL(0xC6, EEPROM.read(addr++));
  TEST_ASSERT_EQUAL(4, EEPROM.read(addr++));
  int b = EEPROM.read(addr++);
  TEST_ASSERT_EQUAL(77, b);
  String deviceId = readStringFrom(addr);
  addr += 2 + deviceId.length();
  String authKey = readStringFrom(addr);
  addr += 2 + authKey.length();
  int refreshSeconds = EEPROM.read(addr++);
  refreshSeconds |= EEPROM.read(addr++) << 8;
  int maxKilowatts = EEPROM.read(addr++);
  maxKilowatts |= EEPROM.read(addr++) << 8;

  TEST_ASSERT_EQUAL_STRING("a1b2c3d4e5f6", deviceId.c_str());
  TEST_ASSERT_EQUAL_STRING("auth-key", authKey.c_str());
  TEST_ASSERT_EQUAL(12, refreshSeconds);
  TEST_ASSERT_EQUAL(15, maxKilowatts);
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_configstore_persist_roundtrip);
  UNITY_END();
}

void loop() {}
