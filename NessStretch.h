#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "Buffer.h"
#include <audio_file/in_file.hpp>
#include "ness_stretch.h"

const int kNumPresets = 1;

enum EParams
 {
    kParamDurMult = 0,
    kParamExtreme,
   kNumParams
 };
 
 enum ECtrlTags
 {
   kCtrlText = 0,
     kCtrlText2,
    kCtrlTagSlider,
    //kCtrlTagSlider1,
    kCtrlTagSlider2,
    kCtrlTagSlider3,
     kSliderLabel2,
     kSliderLabel3,
     kFileButton,
     kCtrlTagTitle1, kCtrlTagTitle2, kCtrlTagTitle3, kCtrlTagTitle4, kCtrlTagTitle5, kCtrlTagTitle6, kCtrlTagTitle7, kCtrlTagTitle8, kCtrlTagTitle9, kCtrlTagTitle10, kCtrlTagTitle11, kCtrlTagTitle12, kCtrlTagTitle13

 };

using namespace iplug;
using namespace igraphics;

class NessStretch final : public Plugin
{
public:
  NessStretch(const InstanceInfo& info);
    Buffer<float> m_buffers[2];
    float* m_process_buffers[2];
    int m_play_counter;
    float m_file_samplerate;
    float m_daw_samplerate;
    int m_current_sample;
    int m_length;
    int m_dur_mult;
    int m_extreme;
    int m_num_slices;
    bool m_playing;
    
    bool m_file_loaded;
    
    NessStretchR* rusties[2];
    
    htl::in_audio_file m_file;
    
    void GetPlaybuf(Buffer<float> inbuf, float* outbuf, int start_frame, int num_samples);

    int m_max_win_size;
    
    void OpenFile(const std::string &str);
#if IPLUG_EDITOR
  void OnParentWindowResize(int width, int height) override;
  bool OnHostRequestingSupportedViewConfiguration(int width, int height) override { return true; }
#endif
  
#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
