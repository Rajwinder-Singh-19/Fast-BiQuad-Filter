#include "ButterFilter.h"
#include "BiQuad.h"
#include "IControls.h"
#include "IPlug_include_in_plug_src.h"

void ButterFilter::OnReset()
{
  const double SAMPLE_RATE_HZ = GetSampleRate();
  double cutoffVal = GetParam(kCutoff)->Value();
  double resoVal = GetParam(kQfactor)->Value();
  FilterType typeVal = (FilterType)GetParam(kFilterType)->Value();

  mLeftFilter.zeroBuffer();
  mRightFilter.zeroBuffer();
  mLeftFilter.setupFilterBuffer(typeVal, cutoffVal, resoVal, SAMPLE_RATE_HZ);
  mRightFilter.setupFilterBuffer(typeVal, cutoffVal, resoVal, SAMPLE_RATE_HZ);
}

void ButterFilter::OnParamChange(int paramIdx)
{
  
  if (paramIdx == kFilterType || paramIdx == kCutoff || paramIdx == kQfactor)
  {
    double cutoffVal = GetParam(kCutoff)->Value();
    double resoVal = GetParam(kQfactor)->Value();
    FilterType typeVal = (FilterType)GetParam(kFilterType)->Value();
    const double SAMPLE_RATE_HZ = GetSampleRate();
    mLeftFilter.setupFilterBuffer(typeVal, cutoffVal, resoVal, SAMPLE_RATE_HZ);
    mRightFilter.setupFilterBuffer(typeVal, cutoffVal, resoVal, SAMPLE_RATE_HZ);
  }

  if (paramIdx == kGain)
  {
    gain = GetParam(kGain)->Value() / 100.0;
  }
}


ButterFilter::ButterFilter(const InstanceInfo& info)
  : iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
  , filterType(FilterType::LowPass)
  , gain(100)
{

  GetParam(kGain)->InitDouble("Gain", 100., 0., 200.0, 0.001, "%");
  GetParam(kFilterType)->InitEnum("Filter Type", (int)FilterType::LowPass, {"LowPass", "HighPass", "BandPass"});
  GetParam(kCutoff)->InitDouble("Cutoff", 20000.0, 20.0, 20000.0, 0.001, "Hz");
  GetParam(kQfactor)->InitDouble("Q", 0.707, 0.01, 20.0, 0.010);

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() { return MakeGraphics(*this, PLUG_WIDTH, 300, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT)); };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_BLUE);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    const IRECT bounds = pGraphics->GetBounds();

    // Layout regions
    const IRECT titleArea = bounds.GetFromTop(80);
    const IRECT menuArea = bounds.GetFromTop(200).GetCentredInside(500, 50);
    const IRECT knobArea = bounds.GetFromTop(400);
    const IRECT footerArea = bounds.GetFromBottom(25).GetHShifted(-10);

    // Title
    pGraphics->AttachControl(new ITextControl(titleArea, "Fast BiQuad Filter!", IText(50)));

    // Filter type selector
    pGraphics->AttachControl(new IVMenuButtonControl(menuArea, kFilterType));

    // Knobs
    const IRECT gainBounds = knobArea.GetGridCell(0, 1, 3);
    const IRECT cutoffBounds = knobArea.GetGridCell(1, 1, 3);
    const IRECT qBounds = knobArea.GetGridCell(2, 1, 3);

    pGraphics->AttachControl(new IVKnobControl(gainBounds.GetCentredInside(100), kGain));

    pGraphics->AttachControl(new IVKnobControl(cutoffBounds.GetCentredInside(100), kCutoff));

    pGraphics->AttachControl(new IVKnobControl(qBounds.GetCentredInside(100), kQfactor));

    // Build info
    WDL_String buildInfoStr;
    GetBuildInfoStr(buildInfoStr, __DATE__, __TIME__);

    pGraphics->AttachControl(new ITextControl(footerArea, buildInfoStr.Get(), DEFAULT_TEXT.WithAlign(EAlign::Far)));
  };
#endif
}

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
