// ds3231_rtc.h — self-contained DS3231 RTC driver for the Marauder Pancake
// (ESP32-C5). Shares the FT6336 touch I2C bus (SDA=9, SCL=10) — no extra
// pins needed. Address 0x68 does not collide with the FT6336 (0x38).
//
// Wiring (Compact DS3231 w/ battery):
//   +  VCC  -> 3V3 (not 5V)
//   D  SDA  -> GPIO 9
//   C  SCL  -> GPIO 10
//   NC      -> leave empty
//   -  GND  -> GND
#pragma once
#ifndef ds3231_rtc_h
#define ds3231_rtc_h

#include <Arduino.h>
#include <Wire.h>

#ifndef RTC_SDA
#define RTC_SDA 9
#endif
#ifndef RTC_SCL
#define RTC_SCL 10
#endif

#define DS3231_ADDR        0x68
#define DS3231_REG_SECONDS 0x00
#define DS3231_REG_STATUS  0x0F
#define DS3231_STATUS_OSF  0x80   // Oscillator Stop Flag — set after power/battery loss

static uint8_t _ds3231_bcd2dec(uint8_t v) { return (uint8_t)(((v >> 4) * 10) + (v & 0x0F)); }
static uint8_t _ds3231_dec2bcd(uint8_t v) { return (uint8_t)(((v / 10) << 4) | (v % 10)); }
static int _ds3231_clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

static bool _ds3231_read(uint8_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom((int)DS3231_ADDR, (int)len);
  for (uint8_t i = 0; i < len; i++)
    buf[i] = Wire.available() ? Wire.read() : 0;
  return true;
}

static bool _ds3231_write(uint8_t reg, const uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(reg);
  for (uint8_t i = 0; i < len; i++) Wire.write(buf[i]);
  return Wire.endTransmission() == 0;
}

// Returns true if a DS3231 responded on the I2C bus. Safe to call even if
// Wire.begin() was already run (e.g. by the FT6336 touch init) — same bus,
// same pins/speed.
static bool ds3231_init() {
  Wire.begin(RTC_SDA, RTC_SCL, 400000U);
  Wire.beginTransmission(DS3231_ADDR);
  bool present = (Wire.endTransmission() == 0);
  Serial.printf("[RTC] DS3231 %s\n", present ? "found" : "not detected");
  return present;
}

// True when the oscillator has stopped since the flag was last cleared —
// dead/missing coin cell, or first-ever power-up. Time read back while
// this is set should not be trusted.
static bool ds3231_lost_power() {
  uint8_t status = 0;
  if (!_ds3231_read(DS3231_REG_STATUS, &status, 1)) return true;
  return (status & DS3231_STATUS_OSF) != 0;
}

static void ds3231_clear_lost_power_flag() {
  uint8_t status = 0;
  if (!_ds3231_read(DS3231_REG_STATUS, &status, 1)) return;
  status &= (uint8_t)~DS3231_STATUS_OSF;
  _ds3231_write(DS3231_REG_STATUS, &status, 1);
}

// Reads wall-clock time. Module is always run in 24-hour mode. Returns
// false on I2C failure.
static bool ds3231_read_time(int *year, int *month, int *day, int *hour, int *minute, int *second) {
  uint8_t d[7];
  if (!_ds3231_read(DS3231_REG_SECONDS, d, 7)) return false;
  *second = _ds3231_bcd2dec(d[0] & 0x7F);
  *minute = _ds3231_bcd2dec(d[1] & 0x7F);
  *hour   = _ds3231_bcd2dec(d[2] & 0x3F);   // bit6=0 → 24h mode
  *day    = _ds3231_bcd2dec(d[4] & 0x3F);
  *month  = _ds3231_bcd2dec(d[5] & 0x1F);
  *year   = 2000 + _ds3231_bcd2dec(d[6]);
  return true;
}

// Writes wall-clock time and clears the lost-power flag so future reads
// are trusted again. dow is 1-7 (arbitrary — the sketch doesn't use it).
static bool ds3231_write_time(int year, int month, int day, int hour, int minute, int second, int dow = 1) {
  uint8_t d[7];
  d[0] = _ds3231_dec2bcd((uint8_t)_ds3231_clamp(second, 0, 59));
  d[1] = _ds3231_dec2bcd((uint8_t)_ds3231_clamp(minute, 0, 59));
  d[2] = _ds3231_dec2bcd((uint8_t)_ds3231_clamp(hour, 0, 23));    // 24h mode, bit6=0
  d[3] = _ds3231_dec2bcd((uint8_t)_ds3231_clamp(dow, 1, 7));
  d[4] = _ds3231_dec2bcd((uint8_t)_ds3231_clamp(day, 1, 31));
  d[5] = _ds3231_dec2bcd((uint8_t)_ds3231_clamp(month, 1, 12));
  d[6] = _ds3231_dec2bcd((uint8_t)_ds3231_clamp(year - 2000, 0, 99));
  if (!_ds3231_write(DS3231_REG_SECONDS, d, 7)) return false;
  ds3231_clear_lost_power_flag();
  return true;
}

#endif // ds3231_rtc_h
