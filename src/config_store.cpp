#include <EEPROM.h>
#include "app_state.h"
#include "board.h"
#include "config_store.h"

static void writeStringToEEPROMLocal(int addrOffset, const String &strToWrite) {
  uint16_t len = strToWrite.length();
  EEPROM.write(addrOffset, len & 0xFF);
  EEPROM.write(addrOffset + 1, (len >> 8) & 0xFF);
  for (int i = 0; i < len; i++) {
    EEPROM.write(addrOffset + 2 + i, strToWrite[i]);
  }
}

void configStoreSetShellyCredentials(const String& deviceId, const String& authKey) {
  shellyDeviceId = deviceId;
  shellyAuthKey = authKey;
}

void configStoreSetBrightness(short b) {
  displayBrightness = b;
}

void configStoreSetPollingInterval(unsigned short seconds) {
  pollingIntervalSeconds = seconds;
  timedTaskIntervalMs = (unsigned long)seconds * 1000UL;
}

void configStoreSetGaugeMaxKilowatts(unsigned short kilowatts) {
  gaugeMaxKilowatts = kilowatts;
  shellyGaugeMaxWatts = (float)kilowatts * 1000.0f;
}

void configStorePersist() {
  int addr = 0;
  EEPROM.write(addr++, 0xC6);
  EEPROM.write(addr++, 4);
  EEPROM.write(addr++, displayBrightness);
  writeStringToEEPROMLocal(addr, shellyDeviceId);
  addr += 2 + shellyDeviceId.length();
  writeStringToEEPROMLocal(addr, shellyAuthKey);
  addr += 2 + shellyAuthKey.length();
  EEPROM.write(addr++, pollingIntervalSeconds & 0xFF);
  EEPROM.write(addr++, (pollingIntervalSeconds >> 8) & 0xFF);
  EEPROM.write(addr++, gaugeMaxKilowatts & 0xFF);
  EEPROM.write(addr++, (gaugeMaxKilowatts >> 8) & 0xFF);
  EEPROM.commit();
}
