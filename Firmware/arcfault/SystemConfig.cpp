#include "SystemConfig.h"

SystemConfigStruct systemConfig;
static bool eepromInitialized = false;

static void ensureEEPROM() {
  if (!eepromInitialized) {
    EEPROM.begin(EEPROM_SIZE);
    eepromInitialized = true;
  }
}

void SystemConfigStruct::saveToEEPROM() {
  ensureEEPROM();
  int addr = 0;
  EEPROM.put(addr, meanThreshold); addr += sizeof(meanThreshold);
  EEPROM.put(addr, samples); addr += sizeof(samples);
  EEPROM.put(addr, samplingFrequency); addr += sizeof(samplingFrequency);
  EEPROM.put(addr, printFFTData); addr += sizeof(printFFTData);
   EEPROM.put(addr, latchState); addr += sizeof(latchState);   // ✅ NEW LINE
  EEPROM.commit();
  Serial.println(F("Config saved to EEPROM"));
}

void SystemConfigStruct::loadFromEEPROM() {
  ensureEEPROM();
  int addr = 0;
  EEPROM.get(addr, meanThreshold); addr += sizeof(meanThreshold);
  EEPROM.get(addr, samples); addr += sizeof(samples);
  EEPROM.get(addr, samplingFrequency); addr += sizeof(samplingFrequency);
  EEPROM.get(addr, printFFTData); addr += sizeof(printFFTData);
  EEPROM.get(addr, latchState); addr += sizeof(latchState);   // ✅ <— ADD THIS
  // === sanity defaults ===
  if (isnan(meanThreshold) || meanThreshold < -100.0f || meanThreshold > 100.0f) meanThreshold = -20.0f;
  if (samples == 0 || samples > 1024) samples = 256;
  if (samplingFrequency < 1000.0f || samplingFrequency > 40000.0f) samplingFrequency = 20000.0f;
  //if (latchState != true && latchState != false) latchState = false; // ✅ safety default
}

void initSystemConfig() {
  systemConfig.loadFromEEPROM();
}
