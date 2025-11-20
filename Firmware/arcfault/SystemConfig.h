#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_SIZE 128

// === Pin Assignments ===
#define ARC_DETECT_PIN_1 14  //relay1
#define ARC_DETECT_PIN_2 25  //relay2
#define TEST_MODE 26
#define RESET_PIN 33
#define ADC_PIN 32


// === Configuration Structure ===
struct SystemConfigStruct {
  float meanThreshold;
  uint16_t samples;
  float samplingFrequency;
  bool printFFTData;
  bool latchState;  // ✅ NEW: Store relay latch state
  
  void loadFromEEPROM();
  void saveToEEPROM();
};

extern SystemConfigStruct systemConfig;

// === Function Prototypes ===
void initSystemConfig();

#endif  // SYSTEM_CONFIG_H
