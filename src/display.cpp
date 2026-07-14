#include "app_state.h"
#include "app.h"
#include "display.h"
#include <qrcode.h>
#include <math.h>
#include "board.h"
#include "icons/wifiOff.h"
#include "icons/hourglassIcon.h"
#include "icons/errorIcon.h"
#include "icons/logo.h"
#include "fonts/NotoSansBold15.h"
#include "fonts/NotoSans15.h"
#include "fonts/ArialBold15.h"
#include "fonts/ArialBold24.h"
#include "fonts/ArialBold46.h"

namespace {
const float kDegToRad = 0.01745329252f;
const int kGaugeStartDeg = 205;
const int kGaugeEndDeg = 335;
const int kGaugeSpanDeg = kGaugeEndDeg - kGaugeStartDeg;
const int kGaugeCx = 120;
const int kGaugeCy = 116;
const int kGaugeRadius = 103;
const int kGaugeThickness = 10;

uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) {
  return tft.color565(r, g, b);
}

float clampFloat(float value, float low, float high) {
  if (value < low) return low;
  if (value > high) return high;
  return value;
}

int angleForRatio(float ratio) {
  return kGaugeStartDeg + (int)(clampFloat(ratio, 0.0f, 1.0f) * kGaugeSpanDeg);
}

void pointOnGauge(int angleDeg, int radius, int& x, int& y) {
  float rad = angleDeg * kDegToRad;
  x = kGaugeCx + (int)(cosf(rad) * radius);
  y = kGaugeCy + (int)(sinf(rad) * radius);
}

void drawThickArc(int startDeg, int endDeg, int radius, int thickness, uint16_t color) {
  if (endDeg < startDeg) {
    int tmp = startDeg;
    startDeg = endDeg;
    endDeg = tmp;
  }
  for (int deg = startDeg; deg <= endDeg; deg++) {
    float rad = deg * kDegToRad;
    for (int offset = -(thickness / 2); offset <= thickness / 2; offset++) {
      int x = kGaugeCx + (int)(cosf(rad) * (radius + offset));
      int y = kGaugeCy + (int)(sinf(rad) * (radius + offset));
      spr.drawPixel(x, y, color);
    }
  }
}

void drawGaugeTicks(uint16_t color) {
  for (int i = 0; i <= 10; i++) {
    float ratio = i / 10.0f;
    int angle = angleForRatio(ratio);
    int outerX, outerY, innerX, innerY;
    pointOnGauge(angle, kGaugeRadius - 12, outerX, outerY);
    pointOnGauge(angle, (i % 5 == 0) ? kGaugeRadius - 24 : kGaugeRadius - 19, innerX, innerY);
    spr.drawLine(innerX, innerY, outerX, outerY, color);
  }
}

uint16_t indicatorColor(float ratio) {
  if (ratio < 0.55f) return rgb(57, 255, 77);
  if (ratio < 0.75f) return rgb(255, 224, 43);
  if (ratio < 0.88f) return rgb(255, 139, 31);
  return rgb(255, 48, 40);
}

void drawConsumptionIndicator(float watts) {
  float ratio = clampFloat(fabsf(watts) / shellyGaugeMaxWatts, 0.0f, 1.0f);
  int angle = angleForRatio(ratio);
  uint16_t color = indicatorColor(ratio);

  drawThickArc(kGaugeStartDeg, angle, kGaugeRadius - 15, 5, color);

  int markerX, markerY;
  pointOnGauge(angle, kGaugeRadius, markerX, markerY);
  spr.fillCircle(markerX, markerY, 9, TFT_BLACK);
  spr.fillCircle(markerX, markerY, 7, TFT_WHITE);
  spr.fillCircle(markerX, markerY, 4, color);
}

String formatDecimal(float value, uint8_t decimals) {
  return String(value, decimals);
}

void formatPower(float watts, String& value, String& unit) {
  if (fabsf(watts) >= 1000.0f) {
    value = formatDecimal(watts / 1000.0f, 2);
    unit = "kW";
  } else {
    value = String((int)roundf(watts));
    unit = "W";
  }
}

void drawGaugeFace(const AppState& state) {
  uint16_t track = rgb(40, 43, 48);
  uint16_t green = rgb(24, 112, 34);
  uint16_t yellow = rgb(126, 103, 18);
  uint16_t orange = rgb(132, 66, 13);
  uint16_t red = rgb(133, 20, 17);

  drawThickArc(kGaugeStartDeg, kGaugeEndDeg, kGaugeRadius, kGaugeThickness + 2, track);
  drawThickArc(kGaugeStartDeg, angleForRatio(0.55f), kGaugeRadius, kGaugeThickness, green);
  drawThickArc(angleForRatio(0.55f), angleForRatio(0.75f), kGaugeRadius, kGaugeThickness, yellow);
  drawThickArc(angleForRatio(0.75f), angleForRatio(0.88f), kGaugeRadius, kGaugeThickness, orange);
  drawThickArc(angleForRatio(0.88f), kGaugeEndDeg, kGaugeRadius, kGaugeThickness, red);
  drawGaugeTicks(rgb(70, 74, 80));

  spr.loadFont(NotoSans15);
  spr.setTextDatum(MC_DATUM);
  spr.setTextColor(TFT_DARKGREY, TFT_BLACK);
  spr.drawString("0", 30, 91);
  spr.drawString(formatDecimal(shellyGaugeMaxWatts / 1000.0f, 1) + "k", 210, 91);
  spr.unloadFont();

  if (state.runtime.hasPowerReading) {
    drawConsumptionIndicator(state.runtime.currentPowerWatts);
  }
}

void renderTickerImpl(const AppState& state) {
  spr.createSprite(240, 135);
  spr.fillSprite(TFT_BLACK);

  drawGaugeFace(state);

  if (state.runtime.hasPowerReading) {
    String value;
    String unit;
    formatPower(state.runtime.currentPowerWatts, value, unit);

    spr.loadFont(ArialBold46);
    spr.setTextDatum(MC_DATUM);
    spr.setTextColor(TFT_WHITE, TFT_BLACK);
    spr.fillRect(58, 59, 124, 52, TFT_BLACK);
    spr.drawString(value, 120, 82);
    spr.unloadFont();

    spr.loadFont(NotoSansBold15);
    spr.setTextDatum(MC_DATUM);
    spr.setTextColor(TFT_DARKGREY, TFT_BLACK);
    spr.drawString(unit, 120, 110);
    spr.unloadFont();
  } else {
    spr.loadFont(ArialBold24);
    spr.setTextDatum(MC_DATUM);
    spr.setTextColor(TFT_DARKGREY, TFT_BLACK);
    spr.drawString("No data", 120, 85);
    spr.unloadFont();
  }

  spr.pushSprite(0, 0);
}
}  // namespace

void renderWifiDisconnected(){
  Serial.println("Reconnect wifi...");
  spr.createSprite(240, 135);
  spr.loadFont(ArialBold24);
  spr.fillSprite(TFT_BLACK);
  spr.pushImage(80, 10, 80, 80, wifiOff);
  spr.setTextDatum(MC_DATUM);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  if(cameFromCaptivePortal)
    spr.drawString("Connecting to WiFi", 120, 115);
  else
    spr.drawString("WiFi disconnected", 120, 115);
  spr.pushSprite(0,0);
  spr.unloadFont();
}

void renderTicker() {
  auto s = getAppState();
  renderTickerImpl(s);
}

void renderGraph() {
  renderTicker();
}

void displayShowCheckingValues(){
  spr.createSprite(240, 135);
  spr.loadFont(ArialBold15);
  spr.fillSprite(TFT_BLACK);
  spr.pushImage(20, 50, 30, 30, hourglassIcon);
  spr.setTextDatum(ML_DATUM);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("Checking Shelly...", 65, 67);
  spr.pushSprite(0, 0);
  spr.unloadFont();
}

void displayShowInvalidParameters(){
  spr.createSprite(240, 135);
  spr.loadFont(ArialBold15);
  spr.fillSprite(TFT_BLACK);
  spr.pushImage(20, 50, 30, 30, errorIcon);
  spr.setTextDatum(ML_DATUM);
  spr.setTextColor(TFT_RED, TFT_BLACK);
  spr.drawString("Invalid parameters", 65, 67);
  spr.pushSprite(0, 0);
  spr.unloadFont();
}

void displayNetworkError(){
  tft.loadFont(NotoSansBold15);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextDatum(ML_DATUM);
  tft.drawString("Error", 5, 28);
  tft.unloadFont();
}

void displaySettingDefaults(){
  spr.createSprite(240, 135);
  spr.loadFont(NotoSansBold15);
  spr.fillSprite(TFT_BLACK);
  spr.setTextDatum(MC_DATUM);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("Setting up", 120, 50);
  spr.drawString("default values", 120, 80);
  spr.pushSprite(0, 0);
  spr.unloadFont();
}

void displayStartCP(){
  spr.createSprite(240, 135);
  spr.loadFont(NotoSansBold15);
  spr.fillSprite(TFT_BLACK);
  spr.setTextDatum(MC_DATUM);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("Please connect to WiFi", 120, 16);
  spr.setTextDatum(MR_DATUM);
  spr.drawString("SSID:", 115, 16 + 25);
  spr.drawString("Password:", 115, 16 + 50);
  spr.setTextDatum(ML_DATUM);
  spr.setTextColor(TFT_GREEN, TFT_BLACK);
  spr.drawString("Shelly-Meter", 125, 16 + 25);
  spr.drawString("12345678", 125, 16 + 50);
  spr.setTextDatum(MC_DATUM);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("and browse to", 120, 16 + 75);
  spr.setTextColor(TFT_YELLOW, TFT_BLACK);
  spr.drawString("http://172.217.28.1", 120, 16 + 100);
  spr.pushSprite(0, 0);
  spr.unloadFont();
}

void displayOtaUpdateScreen(){
  spr.createSprite(240, 135);
  spr.loadFont(ArialBold24);
  spr.fillSprite(TFT_BLACK);
  spr.setTextDatum(MC_DATUM);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("Firmware update", 120, 40);
  spr.drawString("in progress...", 120, 80);
  spr.pushSprite(0,0);
  spr.unloadFont();
}

void displaySplash(const String& version){
  spr.createSprite(240, 135);
  spr.loadFont(NotoSansBold15);
  spr.fillSprite(TFT_BLACK);
  spr.setTextDatum(MC_DATUM);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.pushImage(46, 0, 135, 135, logo);
  spr.setTextDatum(MR_DATUM);
  spr.drawString(version, 220, 125);
  spr.pushSprite(0,0);
  spr.unloadFont();
}

void displayWifiConnected(const String& ip){
  spr.createSprite(240, 135);
  spr.loadFont(ArialBold24);
  spr.fillSprite(TFT_BLACK);
  spr.setTextDatum(MC_DATUM);
  spr.setTextColor(TFT_DARKGREY, TFT_BLACK);
  spr.drawString("WiFi connected", 120, 40);
  spr.setTextColor(TFT_DARKGREEN, TFT_BLACK);
  spr.drawString(ip, 120, 80);
  spr.pushSprite(0,0);
  spr.unloadFont();
}

void displayQRSetup(const String& url){
  spr.createSprite(240, 135);
  const char* qrUrl = url.c_str();
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, qrUrl);
  int border = 2;
  int size = 4;
  int qrSizeInPixels = (qrcode.size + 2 * border) * size;
  int offsetX = 104;
  int offsetY = (tft.height() - qrSizeInPixels) / 2;
  spr.fillSprite(TFT_BLACK);
  spr.fillRect(offsetX, offsetY, qrSizeInPixels, qrSizeInPixels, TFT_BLACK);
  for (int y = 0; y < qrcode.size; y++) {
    for (int x = 0; x < qrcode.size; x++) {
      int color = qrcode_getModule(&qrcode, x, y) ? TFT_WHITE : TFT_BLACK;
      spr.fillRect(offsetX + (x + border) * size, offsetY + (y + border) * size, size, size, color);
    }
  }
  spr.setTextDatum(MC_DATUM);
  spr.loadFont(NotoSansBold15);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.drawString("Scan QR", 60, 28);
  spr.drawString("to setup", 60, 58);
  spr.drawString("Shelly", 60, 88);
  spr.drawString("meter", 60, 118);
  spr.pushSprite(0,0);
  spr.unloadFont();
}
