// ConfigStore: centralized mutation + persistence for config
#pragma once

#include <Arduino.h>

void configStoreSetShellyCredentials(const String& deviceId, const String& authKey);
void configStoreSetBrightness(short b);
void configStoreSetPollingInterval(unsigned short seconds);
void configStoreSetGaugeMaxKilowatts(unsigned short kilowatts);
void configStorePersist();
