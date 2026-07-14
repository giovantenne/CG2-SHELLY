// Configuration storage and validation
#pragma once

#include <Arduino.h>

void resetConfigToDefaults();
boolean isValidNumber(String str);
void handleInvalidParams();
void writeStringToEEPROM(int addrOffset, const String &strToWrite);
String readStringFromEEPROM(int &addrOffset, size_t maxLen = 1600);
void loadConfigFromEEPROM();
bool isValidEmail(String s);
bool isValidShellyDeviceId(String deviceId);
bool isValidShellyServerUri(String serverUri);
bool isValidPollingInterval(String seconds);
bool isValidGaugeMaxKilowatts(String kilowatts);
bool hasShellyCredentials();
