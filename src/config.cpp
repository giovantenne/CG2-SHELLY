#include <EEPROM.h>
#include "app_state.h"
#include "display.h"
#include "config.h"
#include "config_store.h"
#include "board.h"

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  uint16_t len = strToWrite.length();
  EEPROM.write(addrOffset, len & 0xFF);
  EEPROM.write(addrOffset + 1, (len >> 8) & 0xFF);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 2 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(int &addrOffset, size_t maxLen)
{
  uint16_t newStrLen = EEPROM.read(addrOffset) | (EEPROM.read(addrOffset + 1) << 8);
  addrOffset += 2;
  if (newStrLen > maxLen || addrOffset + newStrLen > EEPROM_SIZE) {
    return "";
  }
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + i);
  }
  addrOffset += newStrLen;
  data[newStrLen] = '\0';
  return String(data);
}

bool isValidEmail(String email) {
  email.trim();
  int atIndex = email.indexOf('@');
  int dotAfterAt = email.indexOf('.', atIndex + 2);
  int spaceIndex = email.indexOf(' ');
  return atIndex > 0 &&
         dotAfterAt > atIndex &&
         spaceIndex == -1;
}

bool isValidShellyDeviceId(String deviceId) {
  deviceId.trim();
  if (deviceId.length() == 0 || deviceId.length() > 32) {
    return false;
  }
  for (byte i = 0; i < deviceId.length(); i++) {
    char c = deviceId.charAt(i);
    bool ok = isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || c == 'X' || c == 'x';
    if (!ok) {
      return false;
    }
  }
  return true;
}

bool isValidShellyServerUri(String serverUri) {
  serverUri.trim();
  serverUri.toLowerCase();
  if (serverUri.length() == 0 || serverUri.length() > 128 ||
      !serverUri.endsWith(".shelly.cloud") ||
      serverUri.startsWith(".") || serverUri.endsWith(".")) {
    return false;
  }
  for (byte i = 0; i < serverUri.length(); ++i) {
    char c = serverUri.charAt(i);
    if (!isAlphaNumeric(c) && c != '-' && c != '.') {
      return false;
    }
  }
  return true;
}

bool isValidPollingInterval(String seconds) {
  seconds.trim();
  if (seconds.length() == 0) {
    return false;
  }
  for (byte i = 0; i < seconds.length(); i++) {
    if (!isDigit(seconds.charAt(i))) {
      return false;
    }
  }
  long value = seconds.toInt();
  return value >= 1 && value <= 3600;
}

bool isValidGaugeMaxKilowatts(String kilowatts) {
  kilowatts.trim();
  if (kilowatts.length() == 0) {
    return false;
  }
  for (byte i = 0; i < kilowatts.length(); i++) {
    if (!isDigit(kilowatts.charAt(i))) {
      return false;
    }
  }
  long value = kilowatts.toInt();
  return value >= 1 && value <= 100;
}

bool hasShellyCredentials() {
  return isValidShellyDeviceId(shellyDeviceId) &&
         shellyAuthKey.length() > 0 &&
         isValidShellyServerUri(shellyServerUri);
}

boolean isValidNumber(String str){
  for(byte i=0;i<str.length();i++)
  {
    if(isDigit(str.charAt(i))) return true;
  }
  return false;
}

void resetConfigToDefaults() {
  Serial.println("Setting up default values!");
  displaySettingDefaults();

  displayBrightness = 41;
  shellyDeviceId = "";
  shellyAuthKey = "";
  configStoreSetShellyServerUri("shelly-269-eu.shelly.cloud");
  configStoreSetPollingInterval(5);
  configStoreSetGaugeMaxKilowatts(6);
  hasPowerReading = false;
  shellyOnline = false;

  int addr = 0;
  EEPROM.write(addr++, 0xC6);
  EEPROM.write(addr++, 5);
  EEPROM.write(addr++, displayBrightness);
  writeStringToEEPROM(addr, shellyDeviceId);
  addr += 2 + shellyDeviceId.length();
  writeStringToEEPROM(addr, shellyAuthKey);
  addr += 2 + shellyAuthKey.length();
  EEPROM.write(addr++, pollingIntervalSeconds & 0xFF);
  EEPROM.write(addr++, (pollingIntervalSeconds >> 8) & 0xFF);
  EEPROM.write(addr++, gaugeMaxKilowatts & 0xFF);
  EEPROM.write(addr++, (gaugeMaxKilowatts >> 8) & 0xFF);
  writeStringToEEPROM(addr, shellyServerUri);
  EEPROM.commit();

  delay(3000);
}

void loadConfigFromEEPROM(){
  Serial.println("readConfig");

  int addr = 0;
  uint8_t signature = EEPROM.read(addr++);
  uint8_t version = EEPROM.read(addr++);
  if (signature != 0xC6 || version != 5) {
    resetConfigToDefaults();
    return;
  }

  short b = EEPROM.read(addr++);
  String deviceId = readStringFromEEPROM(addr, 32);
  String authKey = readStringFromEEPROM(addr, 192);
  unsigned short refreshSeconds = EEPROM.read(addr) | (EEPROM.read(addr + 1) << 8);
  addr += 2;
  if (refreshSeconds < 1 || refreshSeconds > 3600) {
    refreshSeconds = 5;
  }
  unsigned short maxKilowatts = EEPROM.read(addr) | (EEPROM.read(addr + 1) << 8);
  addr += 2;
  if (maxKilowatts < 1 || maxKilowatts > 100) {
    maxKilowatts = 6;
  }
  String serverUri = readStringFromEEPROM(addr, 128);

  configStoreSetBrightness(b);
  configStoreSetShellyCredentials(deviceId, authKey);
  configStoreSetShellyServerUri(serverUri);
  configStoreSetPollingInterval(refreshSeconds);
  configStoreSetGaugeMaxKilowatts(maxKilowatts);

  if (!hasShellyCredentials()) {
    resetConfigToDefaults();
  }
}

void handleInvalidParams(){
  displayShowInvalidParameters();
  delay(2000);
  Server.sendHeader("Location", "/setup?valid=false&msg=invalidParameters");
}
