#include <ArduinoJson.h>
#include "app_state.h"
#include "portal.h"
#include "config.h"
#include "config_store.h"
#include "display.h"
#include "api.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "board.h"

namespace {

void selectCurrentValue(AutoConnectSelect& select, const String& value) {
  for (size_t index = 0; index < select.size(); ++index) {
    if (select[index] == value) {
      select.select(value);
      return;
    }
  }
  select.add(value);
  select.select(value);
}

}  // namespace

String initialize2(AutoConnectAux& aux, PageArgument& args) {
  inputShellyDeviceId.value = shellyDeviceId;
  inputShellyAuthKey.value = shellyAuthKey;
  inputShellyServerUri.value = shellyServerUri;
  selectCurrentValue(inputPollingIntervalSeconds, String(pollingIntervalSeconds));
  selectCurrentValue(inputGaugeMaxKilowatts, String(gaugeMaxKilowatts));
  return String();
}

bool startCP(IPAddress ip){
  displayStartCP();
  return true;
}

void rootPage() {
  Server.sendHeader("Location", "/_ac");
  Server.sendHeader("Cache-Control", "no-cache");
  Server.send(301);
}

void deleteAllCredentials(void) {
  AutoConnectCredential credential;
  station_config_t config;
  uint8_t ent = credential.entries();
  Serial.println("Delete all credentials");
  while (ent--) {
    credential.load((int8_t)0, &config);
    credential.del((const char*)&config.ssid[0]);
  }
  resetConfigToDefaults();
  char content[] = "Factory reset; Device is restarting...";
  Server.send(200, "text/plain", content);
  delay(3000);
  // Erase all known WiFi networks from NVS and disconnect
  WiFi.mode(WIFI_STA);
  WiFi.persistent(true);
  WiFi.disconnect(true, true);   // turn off WiFi and erase STA config
  esp_wifi_restore();            // ensure SDK WiFi config cleared
  esp_wifi_stop();               // stop WiFi driver
  esp_wifi_deinit();             // deinit WiFi driver
  // Wipe entire NVS (removes any leftover WiFi entries)
  nvs_flash_erase();
  nvs_flash_init();
  WiFi.softAPdisconnect(true);
  delay(3000);
  ESP.restart();
}

bool detectAP(void) {
  btn2.loop();
  cameFromCaptivePortal = true;
  int16_t  ns = WiFi.scanComplete();
  if (ns == WIFI_SCAN_RUNNING) {
  } else if (ns == WIFI_SCAN_FAILED) {
    if (millis() - wifiScanTimestamp > wifiScanIntervalMs) {
      WiFi.disconnect();
      WiFi.scanNetworks(true, true, false);
    }
  } else {
    Serial.printf("scanNetworks:%d\n", ns);
    int16_t  scanResult = 0;
    while (scanResult < ns) {
      AutoConnectCredential cred;
      station_config_t  staConfig;
      if (cred.load(WiFi.SSID(scanResult++).c_str(), &staConfig) >= 0) {
        Serial.printf("AP %s ready\n", (char*)staConfig.ssid);
        WiFi.scanDelete();
        return false;
      }
    }
    Serial.println("No found known AP");
    WiFi.scanDelete();
    wifiScanTimestamp = millis();
  }
  return true;
}

void handleSaveSettings(void){
  displayShowCheckingValues();

  String newDeviceId = Server.arg("inputShellyDeviceId");
  String newAuthKey = Server.arg("inputShellyAuthKey");
  String newServerUri = Server.arg("inputShellyServerUri");
  String newPollingInterval = Server.arg("inputPollingIntervalSeconds");
  String newGaugeMax = Server.arg("inputGaugeMaxKilowatts");
  newDeviceId.trim();
  newAuthKey.trim();
  newServerUri.trim();
  newServerUri.toLowerCase();
  newPollingInterval.trim();
  newGaugeMax.trim();

  bool requestSuccessful = false;
  String oldDeviceId = shellyDeviceId;
  String oldAuthKey = shellyAuthKey;
  String oldServerUri = shellyServerUri;
  unsigned short oldPollingInterval = pollingIntervalSeconds;
  unsigned short oldGaugeMax = gaugeMaxKilowatts;

  if (isValidShellyDeviceId(newDeviceId) &&
      newAuthKey.length() > 0 &&
      newAuthKey.length() <= 192 &&
      isValidShellyServerUri(newServerUri) &&
      isValidPollingInterval(newPollingInterval) &&
      isValidGaugeMaxKilowatts(newGaugeMax)) {
    configStoreSetShellyCredentials(newDeviceId, newAuthKey);
    configStoreSetShellyServerUri(newServerUri);
    configStoreSetPollingInterval((unsigned short)newPollingInterval.toInt());
    configStoreSetGaugeMaxKilowatts((unsigned short)newGaugeMax.toInt());
    if (fetchShellyStatus()) {
      configStorePersist();
      requestSuccessful = true;
    } else {
      configStoreSetShellyCredentials(oldDeviceId, oldAuthKey);
      configStoreSetShellyServerUri(oldServerUri);
      configStoreSetPollingInterval(oldPollingInterval);
      configStoreSetGaugeMaxKilowatts(oldGaugeMax);
    }
  }

  if (requestSuccessful) {
    delay(3000);
    Server.sendHeader("Location", "/setup?valid=true");
    lastTimedTaskAt = 0; // force immediate refresh
    renderTicker();
  } else {
    handleInvalidParams();
  }
  Server.sendHeader("Cache-Control", "no-cache");
  Server.send(301);
}
