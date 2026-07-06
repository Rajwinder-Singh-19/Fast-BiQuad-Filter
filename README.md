# Fast BiQuad Filter!
A modern C++ VST3 plugin implementing a configurable real-time biquad filter with a lightweight GUI.

Built using:
- Modern C++
- iPlug2
- Steinberg VST3 SDK

Designed with a focus on:
- Low-latency DSP
- Numerical stability
- Real-time safe audio processing
- Separation between the GUI thread and the real-time audio processing thread

###
<img width="1776" height="884" alt="Screenshot (59)" src="https://github.com/user-attachments/assets/9b4b14aa-4f2b-4aa1-af59-88c62453835e" />


# Technical Highlights

This project explores the implementation of real-time digital signal processing by developing a VST3 audio plugin from scratch. The DSP engine performs sample-by-sample biquad filtering while the GUI provides live parameter control without interrupting audio processing. The project emphasizes clean separation between the DSP engine and the user interface on separate threads, deterministic real-time execution, and modern C++ design.

- Real-time VST3 audio plugin written in Modern C++
- Sample-by-sample biquad DSP engine
- Low-pass, high-pass, and band-pass filter implementations
- Real-time safe audio callback (no allocations in the audio thread)
- DSP engine and GUI running on separate threads
- Runtime coefficient recomputation from cutoff frequency and Q
- Supports standard audio sample rates (44.1 kHz, 48 kHz, 96 kHz, etc.)

# Fast BiQuad Filter! : Live audio demo.
###

[Fast BiQuad Demo](https://github.com/user-attachments/assets/cdaf17ed-030a-4228-be73-329b47efd221)

(Click on the crossed-out speaker icon to hear the filter in action)

# Architecture
<img width="1024" height="1536" alt="Architecture" src="https://github.com/user-attachments/assets/f93bf6aa-ebbe-4308-b6b3-588a3dea8cef" />


# BiQuad DSP Engine
## Low-Latency DSP Callback

The audio callback performs deterministic sample-by-sample processing without dynamic memory allocation or blocking operations, making it suitable for real-time execution.

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

## Runtime Coefficient Generation

Filter coefficients are recomputed whenever the cutoff frequency, Q factor, filter type, or sample rate changes, allowing seamless real-time parameter updates while keeping the processing loop lightweight.

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
<details>
<summary> Coefficient Calculations </summary>
  
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
</details> 

## Future Work

- Parametric EQ
- Notch and shelving filters
- SIMD acceleration
- Frequency response visualisation
- Oversampling





