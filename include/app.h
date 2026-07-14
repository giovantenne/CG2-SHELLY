// High-level application state structures
#pragma once

#include <Arduino.h>
#include "app_state.h"

struct ConfigState {
  short displayBrightness = 0;
  String shellyDeviceId;
  String shellyAuthKey;
  unsigned short pollingIntervalSeconds = 5;
  unsigned short gaugeMaxKilowatts = 6;
};

struct RuntimeState {
  float currentPowerWatts = 0.0f;
  float phaseAPowerWatts = 0.0f;
  float phaseBPowerWatts = 0.0f;
  float phaseCPowerWatts = 0.0f;
  String shellyUpdatedAt;
  String lastShellyUpdatedAt;
  bool hasPowerReading = false;
  bool shellyOnline = false;
  bool shellyDataUpdated = false;
  short missingUpdateCount = 0;
  short batteryPercent = 0;
  bool isCharging = false;
  bool cameFromCaptivePortal = false;
  unsigned long wifiScanTimestamp = 0;
  unsigned long lastTimedTaskAt = 0;
};

struct AppState {
  ConfigState config;
  RuntimeState runtime;
};

// Snapshot current globals to a value-type AppState
AppState getAppState();
