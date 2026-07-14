#include "app.h"
#include "app_state.h"

AppState getAppState() {
  AppState s;
  // Config
  s.config.displayBrightness = displayBrightness;
  s.config.shellyDeviceId = shellyDeviceId;
  s.config.shellyAuthKey = shellyAuthKey;
  s.config.pollingIntervalSeconds = pollingIntervalSeconds;
  s.config.gaugeMaxKilowatts = gaugeMaxKilowatts;
  // Runtime
  s.runtime.currentPowerWatts = currentPowerWatts;
  s.runtime.phaseAPowerWatts = phaseAPowerWatts;
  s.runtime.phaseBPowerWatts = phaseBPowerWatts;
  s.runtime.phaseCPowerWatts = phaseCPowerWatts;
  s.runtime.shellyUpdatedAt = shellyUpdatedAt;
  s.runtime.lastShellyUpdatedAt = lastShellyUpdatedAt;
  s.runtime.hasPowerReading = hasPowerReading;
  s.runtime.shellyOnline = shellyOnline;
  s.runtime.shellyDataUpdated = shellyDataUpdated;
  s.runtime.missingUpdateCount = missingUpdateCount;
  s.runtime.batteryPercent = batteryPercent;
  s.runtime.isCharging = isCharging;
  s.runtime.cameFromCaptivePortal = cameFromCaptivePortal;
  s.runtime.wifiScanTimestamp = wifiScanTimestamp;
  s.runtime.lastTimedTaskAt = lastTimedTaskAt;
  return s;
}
