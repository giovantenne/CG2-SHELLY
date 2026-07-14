// API client
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

bool fetchCurrentData();
bool fetchShellyStatus();

#ifdef UNIT_TEST
bool testApplyShellyStatusJson(const String& payload);
#endif
