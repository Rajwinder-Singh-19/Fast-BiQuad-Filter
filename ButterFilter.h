#pragma once

#include "BiQuad.h"
#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kFilterType,
  kCutoff,
  kQfactor,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class ButterFilter final : public Plugin
{
public:
  ButterFilter(const InstanceInfo& info);
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  FilterType filterType;
  BiQuad mLeftFilter;
  BiQuad mRightFilter;
  double gain;

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
