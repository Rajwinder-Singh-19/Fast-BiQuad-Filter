#pragma once
#include "FilterType.h"
#include <cmath>
class BiQuad
{
private:
  double mB0=0.0, mB1=0.0, mB2=0.0, mA1=0.0, mA2=0.0, mX1=0.0, mX2=0.0, mY1=0.0, mY2=0.0;
  FilterType filterType = FilterType::LowPass;
  
  void mSetupLowPass(double alpha, double beta);
  void mSetupHighPass(double alpha, double beta);
  void mSetupBandPass(double alpha, double beta);

public:
  BiQuad() = default;

  void setupFilterBuffer(FilterType filterType, double cutoffFrequencyHz, double QFactor, double sampleRateHz);
  void zeroBuffer();
  double processSample(double sample);
};
