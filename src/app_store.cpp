#include "app_state.h"

void setBrightness(short b) {
  displayBrightness = b;
}

void setBattery(short level, bool isCharging) {
  batteryPercent = level;
  ::isCharging = isCharging;
}

void setPowerReading(float totalWatts, float phaseA, float phaseB, float phaseC, const String& updatedAt, bool online) {
  lastShellyUpdatedAt = shellyUpdatedAt;
  currentPowerWatts = totalWatts;
  phaseAPowerWatts = phaseA;
  phaseBPowerWatts = phaseB;
  phaseCPowerWatts = phaseC;
  shellyUpdatedAt = updatedAt;
  shellyOnline = online;
  hasPowerReading = true;
  shellyDataUpdated = true;
}
