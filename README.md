# Fast BiQuad Filter!
A modern C++ VST3 plugin implementing a configurable real-time biquad filter with a lightweight GUI.

Built using:
- Modern C++
- iPlug2
- Steinberg VST3 SDK

Designed with a focus on:
- low-latency DSP
- numerical stability
- real-time safe audio processing
- supporting audio domain sampling frequencies
- DSP and GUI on separate CPU threads.

This project explores the implementation of real-time digital signal processing by developing a VST3 audio plugin from scratch. The DSP engine performs sample-by-sample biquad filtering while the GUI provides live parameter control without interrupting audio processing. The project emphasizes clean separation between the DSP engine and the user interface on separate CPU threads, deterministic real-time execution, and modern C++ design.
  
###
<img width="1790" height="896" alt="Fast BiQuad Filter" src="https://github.com/user-attachments/assets/3639ae99-466e-4269-b19a-b8a0d3ff6b50" />

# Fast BiQuad Filter! : Live audio demo.
###
https://github.com/user-attachments/assets/48d36102-6627-4a8b-99a1-67cc4aa5f6c4

# BiQuad DSP Engine
## Low-Latency DSP Callback
```cpp
#if IPLUG_DSP
void ButterFilter::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  for (int s = 0; s < nFrames; s++)
  {
    outputs[0][s] = mLeftFilter.processSample(inputs[0][s]) * gain;
    outputs[1][s] = mRightFilter.processSample(inputs[1][s]) * gain;
  }
}
#endif
```
## Buffer Setup
```cpp
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
```
## Variants
### Lowpass Filter
```cpp
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
```
### Highpass Filter
```cpp
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
```
### Bandpass Filter
```cpp
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
```

## Biquad Difference Equation
```cpp
double BiQuad::processSample(double sample)
{
  double y = mB0 * sample + mB1 * mX1 + mB2 * mX2 - mA1 * mY1 - mA2 * mY2;
  mX2 = mX1;
  mX1 = sample;
  mY2 = mY1;
  mY1 = y;

  return y;
}
```




