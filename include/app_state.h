// Centralized global state declarations
#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <FS.h>
using fs::File;
#include "SPIFFS.h"
#include <AutoConnect.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include <ArduinoJson.h>
#include <Pangodream_18650_CL.h>

// Version and API
extern const String firmwareVersion;
extern const String shellyCloudHost;
extern float shellyGaugeMaxWatts;

// Display
extern TFT_eSPI tft;
extern TFT_eSprite spr;

// Tasks and buttons
extern TaskHandle_t Task1;
extern TaskHandle_t Task2;
extern Button2 btn1;
extern Button2 btn2;
extern bool btn1Click;
extern bool btn2Click;

// Web/Portal
extern WebServer Server;
extern AutoConnect Portal;
extern AutoConnectConfig Config;

// AutoConnect pages/components
extern AutoConnectText caption1;
extern AutoConnectSubmit save1;
extern AutoConnectAux aux1;
extern AutoConnectAux aux1Execute;
extern AutoConnectText captionShelly;
extern AutoConnectText captionHr;
extern AutoConnectInput inputShellyDeviceId;
extern AutoConnectInput inputShellyAuthKey;
extern AutoConnectInput inputPollingIntervalSeconds;
extern AutoConnectInput inputGaugeMaxKilowatts;
extern AutoConnectSubmit save2;
extern AutoConnectAux aux2;
extern AutoConnectAux aux3;

// Data/state
extern JsonDocument shellyDoc;
extern float currentPowerWatts;
extern float phaseAPowerWatts;
extern float phaseBPowerWatts;
extern float phaseCPowerWatts;
extern String shellyUpdatedAt;
extern String lastShellyUpdatedAt;
extern bool hasPowerReading;
extern bool shellyOnline;
extern bool shellyDataUpdated;
extern short missingUpdateCount;

extern short displayBrightness;
extern String shellyDeviceId;
extern String shellyAuthKey;
extern unsigned short pollingIntervalSeconds;
extern unsigned short gaugeMaxKilowatts;
extern short batteryPercent;
extern bool isCharging;

// PWM/display backlight
extern const short pwmFreq;
extern const short pwmResolution;
extern const short pwmLedChannelTFT;

// Captive portal flow
extern bool cameFromCaptivePortal;
extern unsigned long wifiScanTimestamp;
extern const unsigned long wifiScanIntervalMs;
extern unsigned long lastTimedTaskAt;
extern unsigned long timedTaskIntervalMs;

// Battery monitor
extern Pangodream_18650_CL BL;
