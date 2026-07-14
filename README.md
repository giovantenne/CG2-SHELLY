# Shelly Pro EM-50 TTGO Display

Firmware for a TTGO T-Display that shows live consumption from a Shelly Pro EM-50 with a tachometer-style gauge.

## Compatibility

This firmware is designed and physically tested with the **Shelly Pro EM-50**. Based on the component schemas in the official Shelly documentation, the current parser is also compatible with these Gen2+ devices:

- **Shelly Pro EM / Pro EM-50**: sums `em1:0` and `em1:1`.
- **Shelly EM Gen3 and Gen4**: sums `em1:0` and `em1:1`.
- **Shelly EM Mini Gen4**: reads `em1:0`.
- **Shelly Pro 3EM, Pro 3EM-400, and Shelly 3EM Gen3**: reads `em:0` in triphase profile or sums `em1:0` through `em1:2` in monophase profile.
- **Shelly Plus PM Mini and Shelly PM Mini Gen3**: reads `pm1:0`.
- Single-channel Gen2+ power-metering relays and plugs exposing `switch:0.apower`, including **Shelly Plus 1PM / Plus 1PM Mini** and **Shelly Pro 1PM**.

Compatibility outside the Pro EM-50 is based on the documented API payloads and has not been verified on physical hardware.

Limitations:

- Gen1 devices use a different status format and are not supported.
- On multi-channel devices based on `switch:*`, only `switch:0` is read; the displayed value is not the total of all channels.
- Power reported only through `cover:*`, `light:*`, or other component types is not supported.
- The gauge scale is configurable from 1 to 100 kW and defaults to 6 kW.

## Features

- Wi-Fi captive portal setup
- Configurable Shelly Cloud `device/status` polling using `device id` and `auth_key`
- Large gauge display with W/kW value, progress arc, and consumption marker
- TTGO T-Display support via PlatformIO

## Setup

On first boot, or after holding button 1 during boot, the device starts a captive portal:

- SSID: `Shelly-Meter`
- Password: `12345678`
- Setup URL shown as QR code on the display

Configure:

- `Device ID`: Shelly device id, for example `a1b2c3d4e5f6`
- `Auth key`: Shelly authorization cloud key used by the HTTP status request.
- `Refresh interval`: HTTP polling interval in seconds (1-3600, default 5).
- `Gauge maximum power`: tachometer full scale in kW (1-100, default 6).

The firmware uses this request shape:

```sh
curl -X POST "https://<server_uri>/device/status" \
  -d "id=<device_id>&auth_key=<auth_key>"
```

The firmware polls the HTTP status endpoint at the configured interval.

## Build

```sh
pio run -e ttgo-t1
```

Upload:

```sh
pio run -e ttgo-t1 -t upload
```

Tests:

```sh
pio test -e ttgo-t1 -v
```

## Code Map

- `src/api.cpp` / `include/api.h`: Shelly HTTP client and JSON status parser
- `src/display.cpp` / `include/display.h`: tachometer UI rendering
- `src/config.cpp` and `src/config_store.cpp`: EEPROM configuration
- `src/portal.cpp`: captive portal settings form
- `src/app_store.cpp`: runtime state mutation helpers
