#include <Arduino.h>
#include "SystemConfig.h"
#include "CLI.h"
#include "ArcFFT.h"

// === Task Handles ===
TaskHandle_t ArcTaskHandle;
TaskHandle_t CLITaskHandle;

// === State Variables ===
volatile bool arcDetected = false;
volatile bool arcLatched = false;

// === Function Prototypes ===
void arcTask(void *parameter);
void cliTask(void *parameter);

void setup() {
  Serial.begin(115200);
  delay(10);

  // === Load saved configuration ===
  initSystemConfig();

  // === Pin Configuration ===
  pinMode(ARC_DETECT_PIN_1, OUTPUT);
  pinMode(ARC_DETECT_PIN_2, OUTPUT);
  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(TEST_MODE, INPUT_PULLUP);

  // === Restore last latch state from EEPROM ===
  arcLatched = systemConfig.latchState;

  // ✅ Immediately apply stored latch state to outputs
  digitalWrite(ARC_DETECT_PIN_1, arcLatched ? HIGH : LOW);
  digitalWrite(ARC_DETECT_PIN_2, arcLatched ? HIGH : LOW);

  Serial.println("=== ARC FAULT DETECTION SYSTEM ===");
  Serial.print("Restored Latch State: ");
  Serial.println(arcLatched ? "LATCHED (ON)" : "CLEARED (OFF)");

  initCLI();
  initArcFFT(systemConfig.samples, systemConfig.samplingFrequency);

  Serial.println("Commands:");
  Serial.println("  SETTH <value>      Set threshold (-100 to +100 dB)");
  Serial.println("  SETSAMP <value>    Set sample size (32–1024)");
  Serial.println("  SETFREQ <Hz>       Set sampling frequency (1k–50k)");
  Serial.println("  PRINTFFT ON/OFF    Toggle FFT data printing");
  Serial.println("  GETCONF            Show current configuration");
  Serial.println("  HELP               Show all commands");
  Serial.println("----------------------------------");

  // === Create RTOS Task for ARC Detection ===
  xTaskCreatePinnedToCore(
    arcTask,
    "ArcTask",
    4096,
    NULL,
    1,
    &ArcTaskHandle,
    1);

  // === Create RTOS Task for CLI Processing ===
  xTaskCreatePinnedToCore(
    cliTask,
    "CLITask",
    4096,
    NULL,
    1,
    &CLITaskHandle,
    1);

  Serial.println("System initialized with FreeRTOS tasks.");
}


void loop() {
  // --- FFT Reading only ---
  float meanAmplitude = performFFT(systemConfig.printFFTData);
  bool detected = (meanAmplitude >= systemConfig.meanThreshold);

  // === Apply test mode override ===
  bool testModeActive = (digitalRead(TEST_MODE) == LOW);  // LOW = active
  if (testModeActive) {
    detected = true;  // Force arc detection
  }

  arcDetected = detected;  // Update global flag for the RTOS task
  delay(100);              // Allow CPU time for other tasks
}

// === RTOS Task: Handles ARC latching, reset, and outputs ===
void arcTask(void *parameter) {
  unsigned long bootTime = millis();  // record boot time

  for (;;) {
    // === Detection logic ===
    if (arcDetected) {
      if (!arcLatched) {
        arcLatched = true;
        systemConfig.latchState = true;
        // Turn off FFT printing when arc is detected
        systemConfig.printFFTData = false;

        systemConfig.saveToEEPROM();
        Serial.println("⚠️ ARC DETECTED — latched and saved to EEPROM!");
      }
    }

    // === Reset logic (ignore first 2 seconds after boot) ===
    if (millis() - bootTime > 2000) {
      if (digitalRead(RESET_PIN) == LOW) {
        if (arcLatched || systemConfig.latchState) {
          arcLatched = false;
          systemConfig.latchState = false;
          // Allow FFT printing again after reset
          systemConfig.printFFTData = true;
          
          systemConfig.saveToEEPROM();
          Serial.println("🔄 ARC RESET — state cleared in EEPROM");
        }
        delay(300);  // Debounce delay
      }
    }

    // === Drive outputs ===
    digitalWrite(ARC_DETECT_PIN_1, arcLatched ? HIGH : LOW);
    digitalWrite(ARC_DETECT_PIN_2, arcLatched ? HIGH : LOW);

    // === Status print ===
    if (arcLatched)
      Serial.println("⚠️ ARC DETECTED (latched)!");
    else
      Serial.println("Normal");

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}



// === RTOS Task: Handles CLI commands ===
void cliTask(void *parameter) {
  for (;;) {
    processCLI();                   // Process serial commands
    vTaskDelay(pdMS_TO_TICKS(50));  // Small delay to avoid busy-looping
  }
}
