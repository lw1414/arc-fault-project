#ifndef ARCFFT_H
#define ARCFFT_H

#include <Arduino.h>
#include "arduinoFFT.h"

void initArcFFT(uint16_t samples, float samplingFrequency);
float performFFT(bool printFFTData);
void reinitArcFFT(uint16_t samples, float samplingFrequency); // ✅ new function

#endif
