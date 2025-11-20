#ifndef CLI_H
#define CLI_H

#include <Arduino.h>
#include "SystemConfig.h"
#include "arduinoFFT.h"


// Initializes CLI (optional future setup)
void initCLI();

// Handles all serial commands (to be called from loop)
void processCLI();

#endif
