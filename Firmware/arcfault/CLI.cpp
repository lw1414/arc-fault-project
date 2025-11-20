#include "CLI.h"
#include "SystemConfig.h"
#include "ArcFFT.h"

void initCLI() {
  // reserved for future CLI setup
}

void processCLI() {
  if (!Serial.available()) return;

  String input = Serial.readStringUntil('\n');
  input.trim();

  // Normalize input
  input.replace("=", " ");
  input.trim();

  // === SETTH ===
  if (input.startsWith("SETTH")) {
    String param = input.substring(6);
    param.trim();

    if (param == "?") {
      Serial.print("Current Mean Threshold: ");
      Serial.print(systemConfig.meanThreshold);
      Serial.println(" dB");
    } 
    else if (param.length() > 0) {
      float val = param.toFloat();
      if (val >= -100.0 && val <= 100.0) {
        systemConfig.meanThreshold = val;
        systemConfig.saveToEEPROM();
        Serial.print("Mean Threshold updated to: ");
        Serial.print(systemConfig.meanThreshold);
        Serial.println(" dB");
      } else {
        Serial.println("Invalid value. Range: -100 to +100 dB");
      }
    } 
    else {
      Serial.println("Usage: SETTH=<value> or SETTH=?");
    }
  }

  // === SETSAMP ===
  else if (input.startsWith("SETSAMP")) {
    String param = input.substring(8);
    param.trim();

    if (param == "?") {
      Serial.print("Current Samples: ");
      Serial.println(systemConfig.samples);
    } 
    else if (param.length() > 0) {
      int val = param.toInt();
      if (val > 32 && val <= 1024) {
        systemConfig.samples = val;
        reinitArcFFT(systemConfig.samples, systemConfig.samplingFrequency);
        systemConfig.saveToEEPROM();
        Serial.print("Samples updated to: ");
        Serial.println(systemConfig.samples);
      } else {
        Serial.println("Invalid value. Range: 32–1024");
      }
    } 
    else {
      Serial.println("Usage: SETSAMP=<value> or SETSAMP=?");
    }
  }

  // === SETFREQ ===
  else if (input.startsWith("SETFREQ")) {
    String param = input.substring(8);
    param.trim();

    if (param == "?") {
      Serial.print("Current Sampling Frequency: ");
      Serial.print(systemConfig.samplingFrequency);
      Serial.println(" Hz");
    } 
    else if (param.length() > 0) {
      float val = param.toFloat();
      if (val >= 1000 && val <= 50000) {
        systemConfig.samplingFrequency = val;
        reinitArcFFT(systemConfig.samples, systemConfig.samplingFrequency);
        systemConfig.saveToEEPROM();
        Serial.print("Sampling Frequency updated to: ");
        Serial.println(systemConfig.samplingFrequency);
      } else {
        Serial.println("Invalid value. Range: 1kHz–50kHz");
      }
    } 
    else {
      Serial.println("Usage: SETFREQ=<Hz> or SETFREQ=?");
    }
  }

  // === PRINTFFT ===
  else if (input.startsWith("PRINTFFT")) {
    String param = input.substring(9);
    param.trim();

    if (param == "?") {
      Serial.print("Current FFT printing state: ");
      Serial.println(systemConfig.printFFTData ? "ON" : "OFF");
    } 
    else if (param.equalsIgnoreCase("ON")) {
      systemConfig.printFFTData = true;
      systemConfig.saveToEEPROM();
      Serial.println("FFT data printing: ON");
    } 
    else if (param.equalsIgnoreCase("OFF")) {
      systemConfig.printFFTData = false;
      systemConfig.saveToEEPROM();
      Serial.println("FFT data printing: OFF");
    } 
    else {
      Serial.println("Usage: PRINTFFT=ON/OFF or PRINTFFT=?");
    }
  }

  // === LATCHSTATE === 🆕
  else if (input.startsWith("LATCHSTATE")) {
    String param = input.substring(11);
    param.trim();

    if (param == "?") {
      Serial.print("Current Latch State: ");
      Serial.println(systemConfig.latchState ? "LATCHED (ON)" : "CLEARED (OFF)");
    } 
    else if (param.equalsIgnoreCase("ON")) {
      systemConfig.latchState = true;
      systemConfig.saveToEEPROM();
      Serial.println("Latch state set to: ON");
    } 
    else if (param.equalsIgnoreCase("OFF")) {
      systemConfig.latchState = false;
      systemConfig.saveToEEPROM();
      Serial.println("Latch state set to: OFF");
    } 
    else {
      Serial.println("Usage: LATCHSTATE=ON/OFF or LATCHSTATE=?");
    }
  }

  // === GETCONF ===
  else if (input.equalsIgnoreCase("GETCONF")) {
    Serial.println("=== Current Configuration ===");
    Serial.print("Mean Threshold: "); Serial.println(systemConfig.meanThreshold);
    Serial.print("Samples: "); Serial.println(systemConfig.samples);
    Serial.print("Sampling Frequency: "); Serial.println(systemConfig.samplingFrequency);
    Serial.print("Print FFT: "); Serial.println(systemConfig.printFFTData ? "ON" : "OFF");
    Serial.print("Latch State: "); Serial.println(systemConfig.latchState ? "ON" : "OFF");
  }

  // === HELP ===
  else if (input.equalsIgnoreCase("HELP")) {
    Serial.println("=== Available Commands ===");
    Serial.println("SETTH=<value> / ?       Set or query threshold (-100 to +100 dB)");
    Serial.println("SETSAMP=<value> / ?     Set or query sample size (32–1024)");
    Serial.println("SETFREQ=<Hz> / ?        Set or query sampling frequency (1k–50k)");
    Serial.println("PRINTFFT=ON/OFF / ?     Toggle or query FFT data printing");
    Serial.println("LATCHSTATE=ON/OFF / ?   Set, clear, or query saved latch state");
    Serial.println("GETCONF                 Show all current settings");
    Serial.println("HELP                    List all commands");
  }

  // === Unknown Command ===
  else {
    Serial.println("Unknown command. Type 'HELP' for list.");
  }
}
