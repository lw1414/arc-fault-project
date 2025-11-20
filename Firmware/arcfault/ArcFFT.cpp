#include "ArcFFT.h"
#include "SystemConfig.h"

static float *vReal = nullptr;
static float *vImag = nullptr;
static ArduinoFFT<float> *FFT = nullptr;
static unsigned long sampling_period_us;
static unsigned long microseconds;

void initArcFFT(uint16_t samples, float samplingFrequency) {
  vReal = new float[samples];
  vImag = new float[samples];
  FFT = new ArduinoFFT<float>(vReal, vImag, samples, samplingFrequency, true);
  sampling_period_us = round(1000000.0 / samplingFrequency);
}

void reinitArcFFT(uint16_t samples, float samplingFrequency) {
  if (vReal) delete[] vReal;
  if (vImag) delete[] vImag;
  if (FFT) delete FFT;
  initArcFFT(samples, samplingFrequency);
}

float performFFT(bool printFFTData) {
  microseconds = micros();
  for (int i = 0; i < systemConfig.samples; i++) {
    vReal[i] = analogRead(ADC_PIN);
    vImag[i] = 0;
    while (micros() - microseconds < sampling_period_us);
    microseconds += sampling_period_us;
  }

  FFT->windowing(FFTWindow::Hamming, FFTDirection::Forward);
  FFT->compute(FFTDirection::Forward);
  FFT->complexToMagnitude();

  float freqResolution = systemConfig.samplingFrequency / systemConfig.samples;
  float meanAmplitude_dB = 0;
  int count = 0;

  float maxAmp = 0;
  for (int i = 1; i < systemConfig.samples / 2; i++) {
    if (vReal[i] > maxAmp) maxAmp = vReal[i];
  }
  if (maxAmp < 1e-6) maxAmp = 1e-6;

  if (printFFTData) Serial.println("--- Spectrum (in dB) ---");

  for (int i = 1; i < systemConfig.samples / 2; i++) {
    float freq = i * freqResolution;
    float amplitude = vReal[i];
    float amplitude_dB = 20.0 * log10(amplitude / maxAmp + 1e-12);

    if (freq >= 1000 && freq <= 10000) {
      meanAmplitude_dB += amplitude_dB;
      count++;
    }

    if (printFFTData) {
      Serial.print(freq, 1);
      Serial.print("\t");
      Serial.println(amplitude_dB, 2);
    }
  }

  if (count > 0) meanAmplitude_dB /= count;
  else meanAmplitude_dB = -999;

  // === Include mean amplitude inside the FFT print block ===
  if (printFFTData) {
    Serial.print("MEAN AMPLITUDE: ");
    Serial.print(meanAmplitude_dB, 2);
    Serial.println(" dB");
  }

  return meanAmplitude_dB;
}
