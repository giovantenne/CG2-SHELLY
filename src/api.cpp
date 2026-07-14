#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

#include "app_state.h"
#include "display.h"
#include "app_store.h"
#include "api.h"

namespace {
const char* kFormMimeType = "application/x-www-form-urlencoded";

bool readNumber(JsonVariantConst value, float& out) {
  if (value.isNull()) {
    return false;
  }
  out = value.as<float>();
  return true;
}

bool readObjectNumber(JsonObjectConst object, const char* key, float& out) {
  return readNumber(object[key], out);
}

String statusTimestamp(JsonObjectConst status) {
  const char* updated = status["_updated"] | "";
  if (updated[0] != '\0') {
    return String(updated);
  }
  float serial = 0.0f;
  if (readObjectNumber(status, "serial", serial)) {
    return String(serial, 2);
  }
  return String(millis());
}

bool applyShellyStatus(JsonObjectConst status, bool online, const String& updatedAt) {
  float total = currentPowerWatts;
  float phaseA = phaseAPowerWatts;
  float phaseB = phaseBPowerWatts;
  float phaseC = phaseCPowerWatts;
  bool found = false;
  bool foundTotal = false;
  bool foundPhase = false;

  JsonObjectConst em = status["em:0"].as<JsonObjectConst>();
  if (!em.isNull()) {
    float value = 0.0f;
    if (readObjectNumber(em, "total_act_power", value)) {
      total = value;
      found = true;
      foundTotal = true;
    }
    if (readObjectNumber(em, "a_act_power", value)) {
      phaseA = value;
      found = true;
      foundPhase = true;
    }
    if (readObjectNumber(em, "b_act_power", value)) {
      phaseB = value;
      found = true;
      foundPhase = true;
    }
    if (readObjectNumber(em, "c_act_power", value)) {
      phaseC = value;
      found = true;
      foundPhase = true;
    }
  }

  JsonObjectConst em1 = status["em1:0"].as<JsonObjectConst>();
  if (!em1.isNull()) {
    float value = 0.0f;
    if (readObjectNumber(em1, "act_power", value)) {
      phaseA = value;
      found = true;
      foundPhase = true;
    }
  }

  em1 = status["em1:1"].as<JsonObjectConst>();
  if (!em1.isNull()) {
    float value = 0.0f;
    if (readObjectNumber(em1, "act_power", value)) {
      phaseB = value;
      found = true;
      foundPhase = true;
    }
  }

  em1 = status["em1:2"].as<JsonObjectConst>();
  if (!em1.isNull()) {
    float value = 0.0f;
    if (readObjectNumber(em1, "act_power", value)) {
      phaseC = value;
      found = true;
      foundPhase = true;
    }
  }

  JsonObjectConst pm1 = status["pm1:0"].as<JsonObjectConst>();
  if (!pm1.isNull()) {
    float value = 0.0f;
    if (readObjectNumber(pm1, "apower", value) || readObjectNumber(pm1, "act_power", value)) {
      total = value;
      found = true;
      foundTotal = true;
    }
  }

  JsonObjectConst sw = status["switch:0"].as<JsonObjectConst>();
  if (!sw.isNull()) {
    float value = 0.0f;
    if (readObjectNumber(sw, "apower", value)) {
      total = value;
      found = true;
      foundTotal = true;
    }
  }

  if (foundPhase && !foundTotal) {
    total = phaseA + phaseB + phaseC;
  }

  if (!found) {
    return false;
  }

  setPowerReading(total, phaseA, phaseB, phaseC, updatedAt, online);
  missingUpdateCount = 0;
  return true;
}

bool parseDeviceStatusResponse(const String& payload) {
  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print("Shelly JSON deserialization failed: ");
    Serial.println(error.c_str());
    return false;
  }

  if (!doc["isok"].as<bool>()) {
    return false;
  }

  JsonObjectConst data = doc["data"].as<JsonObjectConst>();
  JsonObjectConst status = data["device_status"].as<JsonObjectConst>();
  if (status.isNull()) {
    return false;
  }

  bool online = data["online"] | true;
  return applyShellyStatus(status, online, statusTimestamp(status));
}

String formEncode(const String& value) {
  String encoded;
  encoded.reserve(value.length() * 3);
  const char* hex = "0123456789ABCDEF";
  for (size_t i = 0; i < value.length(); i++) {
    char c = value.charAt(i);
    bool safe = isAlphaNumeric(c) || c == '-' || c == '_' || c == '.' || c == '~';
    if (safe) {
      encoded += c;
    } else {
      encoded += '%';
      encoded += hex[(c >> 4) & 0x0F];
      encoded += hex[c & 0x0F];
    }
  }
  return encoded;
}

}  // namespace

bool fetchShellyStatus() {
  if (shellyDeviceId.length() == 0 || shellyAuthKey.length() == 0) {
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(12000);

  HTTPClient http;
  String url = "https://" + shellyCloudHost + "/device/status";
  if (!http.begin(client, url)) {
    return false;
  }
  http.addHeader("Content-Type", kFormMimeType);

  String body = "id=" + formEncode(shellyDeviceId) + "&auth_key=" + formEncode(shellyAuthKey);
  int httpResponseCode = http.POST(body);
  Serial.printf("[HTTP] Shelly status: %d\n", httpResponseCode);
  if (httpResponseCode <= 0) {
    http.end();
    return false;
  }

  String payload = http.getString();
  http.end();
  return parseDeviceStatusResponse(payload);
}

bool fetchCurrentData() {
  return fetchShellyStatus();
}

#ifdef UNIT_TEST
bool testApplyShellyStatusJson(const String& payload) {
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    return false;
  }
  JsonObjectConst status = doc.as<JsonObjectConst>();
  return applyShellyStatus(status, true, statusTimestamp(status));
}
#endif
