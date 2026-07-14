// Definitions for global state
#include "app_state.h"
#include "board.h"

// Version and API
const String shellyCloudHost = "shelly-269-eu.shelly.cloud";
float shellyGaugeMaxWatts = 6000.0f;

// Display
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

// Tasks and buttons
TaskHandle_t Task1;
TaskHandle_t Task2;
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);
bool btn1Click = false;
bool btn2Click = false;

// Web/Portal
WebServer Server;
AutoConnect Portal(Server);
AutoConnectConfig Config;

// AutoConnect components
ACText(caption1, "Do you want to perform a factory reset?");
ACSubmit(save1, "Yes, reset the device", "/delconnexecute");
AutoConnectAux aux1("/delconn", "Reset", true, {caption1, save1});
AutoConnectAux aux1Execute("/delconnexecute", "Wifi reset", false);
ACText(captionShelly, "<hr><strong>Shelly Cloud</strong><br><small>Enter the Shelly Pro EM-50 device id and authorization cloud key.</small><br>");
ACText(captionHr, "<hr>");
ACInput(inputShellyDeviceId, "", "Device ID", "", "a1b2c3d4e5f6");
ACInput(inputShellyAuthKey, "", "Auth key", "", "", AC_Tag_BR, AC_Input_Password);
ACInput(inputPollingIntervalSeconds, "5", "Refresh interval (seconds)", "", "5");
ACInput(inputGaugeMaxKilowatts, "6", "Gauge maximum power (kW)", "", "6");
ACSubmit(save2, "Save", "/setupexecute");
AutoConnectAux aux2("/setup", "Settings", true, {captionShelly, inputShellyDeviceId, inputShellyAuthKey, inputPollingIntervalSeconds, inputGaugeMaxKilowatts, captionHr, save2});
AutoConnectAux aux3("/setupexecute", "", false);

// Data/state
JsonDocument shellyDoc;
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

short displayBrightness = 21;
String shellyDeviceId;
String shellyAuthKey;
unsigned short pollingIntervalSeconds = 5;
unsigned short gaugeMaxKilowatts = 6;

short batteryPercent = 0;
bool isCharging = false;

// PWM/display backlight
const short pwmFreq = 5000;
const short pwmResolution = 8;
const short pwmLedChannelTFT = 0;

// Captive portal flow
bool cameFromCaptivePortal = false;
unsigned long wifiScanTimestamp = 0;
const unsigned long wifiScanIntervalMs = 120 * 1000;
unsigned long lastTimedTaskAt = -1000000000;
unsigned long timedTaskIntervalMs = 5000;

// Battery monitor
Pangodream_18650_CL BL(ADC_PIN, CONV_FACTOR, READS);
