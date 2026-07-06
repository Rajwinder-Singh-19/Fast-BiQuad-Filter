#include "BiQuad.h"
#include <numbers>
void BiQuad::zeroBuffer()
{
  mB0 = 0.0;
  mB1 = 0.0;
  mB2 = 0.0;
  mA1 = 0.0;
  mA2 = 0.0;
  mX1 = 0.0;
  mX2 = 0.0;
  mY1 = 0.0;
  mY2 = 0.0;
}

void BiQuad::mSetupLowPass(double alpha, double beta)
{
  double b0 = 0.5 * (1 - beta);
  double b1 = 2.0 * b0;
  double b2 = b0;

  double a1 = -2.0 * beta;

  mB0 = b0 / (1.0 + alpha);
  mB1 = b1 / (1.0 + alpha);
  mB2 = b2 / (1.0 + alpha);

  mA1 = a1 / (1.0 + alpha);
  mA2 = (1 - alpha) / (1 + alpha);
}

void BiQuad::mSetupHighPass(double alpha, double beta)
{
  double b0 = 0.5 * (1 + beta);
  double b1 = -2.0 * b0;
  double b2 = b0;

  double a1 = -2.0 * beta;

  mB0 = b0 / (1.0 + alpha);
  mB1 = b1 / (1.0 + alpha);
  mB2 = b2 / (1.0 + alpha);

  mA1 = a1 / (1.0 + alpha);
  mA2 = (1 - alpha) / (1 + alpha);
}

void BiQuad::mSetupBandPass(double alpha, double beta)
{
  double b0 = alpha;
  double b1 = 0;
  double b2 = -b0;

  double a1 = -2.0 * beta;

  mB0 = b0 / (1.0 + alpha);
  mB1 = b1 / (1.0 + alpha);
  mB2 = b2 / (1.0 + alpha);

  mA1 = a1 / (1.0 + alpha);
  mA2 = (1 - alpha) / (1 + alpha);
}

void BiQuad::setupFilterBuffer(FilterType filterType, double cutoffFrequencyHz, double QFactor, double sampleRateHz)
{
  double omega = 2.0 * std::numbers::pi * cutoffFrequencyHz / sampleRateHz;
  double alpha = std::sin(omega) / (2.0 * QFactor);
  double beta = std::cos(omega);

  switch (filterType)
  {
  case FilterType::LowPass:
    mSetupLowPass(alpha, beta);
    break;
  case FilterType::HighPass:
    mSetupHighPass(alpha, beta);
    break;
  case FilterType::BandPass:
    mSetupBandPass(alpha, beta);
    break;
  default:
    mSetupLowPass(alpha, beta);
    break;
  }
}

double BiQuad::processSample(double sample)
{
  double y = mB0 * sample + mB1 * mX1 + mB2 * mX2 - mA1 * mY1 - mA2 * mY2;
  mX2 = mX1;
  mX1 = sample;
  mY2 = mY1;
  mY1 = y;

  return y;
}
