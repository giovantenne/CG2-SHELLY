// Mutating operations on global app state
#pragma once

#include <Arduino.h>

void setBrightness(short b);
void setBattery(short level, bool isCharging);
void setPowerReading(float totalWatts, float phaseA, float phaseB, float phaseC, const String& updatedAt, bool online);
