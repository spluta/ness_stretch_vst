#include "NessStretch.h"
#include "IPlug_include_in_plug_src.h"
//#include "../HISSTools_PluginLib/HISSTools_UI/HISSTools_Controls.hpp"

//#include "Harker/HISSTools_Library/AudioFile/BaseAudioFile.h"
#include "ness_stretch_static/ness_stretch.h"
#include "Buffer.h"

#if IPLUG_EDITOR
#include "IControls.h"
#include "Buffer.h"
#endif

//class HISSTools_GFileSelector : public HISSTools_FileSelector
//{
//
//public:
//
//  HISSTools_GFileSelector(HISSToolsGranular *plug, double x, double y, double w, double h, EFileAction action, char* dir = "", char* extensions = "", const char *type = 0, HISSTools_Design_Scheme *designScheme = &DefaultDesignScheme)
//  :HISSTools_FileSelector(kNoParameter, x, y, w, h, action, dir, extensions, type, designScheme, "Select"), mPlug(plug) {}
//
//private:
//
//  void reportToPlug() override
//  {
//    mPlug->SelectFile(GetLastSelectedFileForPlug().Get());
//  }
//
//  HISSToolsGranular *mPlug;
//};

void NessStretch::OpenFile(const std::string &file_name){
    
    m_playing = false;
    m_file.open(file_name);
    
    m_length = (int)m_file.frames();
    m_length = m_length > 30000000 ? 30000000 : m_length;
    m_buffers[0].resize(m_length);
    m_buffers[1].resize(m_length);
    
    m_file.read_channel(m_buffers[0].data(), m_length, 0);
    
    m_dur_mult = 100;
    m_extreme = 0;
    m_daw_samplerate = GetSampleRate();
    
    m_max_win_size = (m_daw_samplerate>50000.0) ? 131072:65536;
    
    int num_slices = (m_daw_samplerate>50000.0) ? 10: 9;
    
    for (int i = 0; i<2; i++)
        rusties[i] = ness_stretch_new((float)m_dur_mult, m_max_win_size, 1, num_slices, m_extreme, 8192);
    
    m_file.seek(0);
    
    if (m_file.channels() > 1)
        m_file.read_channel(m_buffers[1].data(), m_length, 1);
    else
        m_buffers[1] = m_buffers[0];
    
    m_file_samplerate = m_file.sampling_rate();
    
    for (int i=0; i<2; i++)
        m_process_buffers[i] = (float*)malloc(m_max_win_size * sizeof(float));
    m_play_counter = m_max_win_size;
    m_current_sample = 0;
    
    m_file_loaded = true;
}


NessStretch::NessStretch(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
    m_file_loaded = false;
    
    OpenFile("/Users/spluta1/Library/Application Support/SuperCollider/sounds/Man In The Mirror.aif");
    
    //-----------------Graphics
    
    //GetParam(kParamGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");
    GetParam(kParamDurMult)->InitDouble("Dur Mult", 100.0, 1.0, 1000.0, 1.0, "X");
    GetParam(kParamExtreme)->InitDouble("Extreme Mode", 0., 0., 3.0, 1.0, "extra");

  #if IPLUG_EDITOR // http://bit.ly/2S64BDd
    mMakeGraphicsFunc = [&]() {
      return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS);
    };
    
    mLayoutFunc = [&](IGraphics* pGraphics) {

  layout:
      if (pGraphics->NControls()) {
        const IRECT bounds = pGraphics->GetBounds();
        const IRECT innerBounds = bounds.GetPadded(-10.f);
        const IRECT sliderBounds = innerBounds.GetFromLeft(100).GetFromTop(40);
        const IRECT sliderLabels = sliderBounds.GetFromLeft(400);
          //const IRECT sliderBounds = MakeXYWH(
        const IRECT textBounds = innerBounds.GetFromBRHC(500, 40);
        const IRECT titleBounds = innerBounds.GetFromLeft(350).GetFromTop(25);
        
        pGraphics->GetBackgroundControl()->As<IPanelControl>()->SetPattern(IColor(255,0,0,0));
        pGraphics->GetBackgroundControl()->SetTargetAndDrawRECTs(bounds);
        //pGraphics->GetControlWithTag(kCtrlTagSlider1)->SetTargetAndDrawRECTs(sliderBounds);
          
        pGraphics->GetControlWithTag(kCtrlTagSlider2)->SetTargetAndDrawRECTs(sliderBounds.GetVShifted(sliderBounds.H() * 2));
        pGraphics->GetControlWithTag(kCtrlTagSlider3)->SetTargetAndDrawRECTs(sliderBounds.GetVShifted(sliderBounds.H() * 3));
          
        pGraphics->GetControlWithTag(kSliderLabel2)->SetTargetAndDrawRECTs(sliderLabels.GetVShifted(sliderBounds.H()*2).GetHShifted(100));
        pGraphics->GetControlWithTag(kSliderLabel3)->SetTargetAndDrawRECTs(sliderLabels.GetVShifted(sliderBounds.H()*3).GetHShifted(100));
          
          //promptForFile
          //for (int i = 0; i<14; i++)
              //pGraphics->GetControlWithTag(kCtrlTagTitle1)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist)));
          
          //pGraphics->GetControlWithTag(kFileButton)->SetTargetAndDrawRECTs(sliderBounds.GetVShifted(sliderBounds.H() * 3));
          
//          pGraphics->PromptForFile(<#WDL_String &fileName#>, <#WDL_String &path#>);
//          
//          mLayoutFunc = [&](IGraphics* pGraphics) {
//              // Add a button control
//              pGraphics->AttachControl(new IVButtonControl(b.GetCentredInside(100), [=](IControl* pControl) {
//                  // Define the action when the button is clicked
//                  WDL_String file;
//                  pGraphics->PromptForFile(file, EFileAction::Open, "wav", "audio files");
//                  
//                  if (file.GetLength()) {
//                      // Handle the file here
//                      // For example, load the audio file or something similar
//                  }
//              }, "Open File"));
//          };
          
              int dist1 = 0;
              int dist2 = 25;
              int dist3 = 50;
              int dist4 = 75;
              int dist5 = 100;
              int dist6 = 125;
              int dist7 = 150;
              int dist8 = 175;
              int dist9 = 200;
              int dist10 = 225;
              int dist11 = 250;
              int dist12 = 275;
              int dist13 = 325;
                int dist14 = 375;
          
          pGraphics->GetControlWithTag(kCtrlTagTitle1)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist1)));
          pGraphics->GetControlWithTag(kCtrlTagTitle2)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist2)));
          pGraphics->GetControlWithTag(kCtrlTagTitle3)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist3)));
          pGraphics->GetControlWithTag(kCtrlTagTitle4)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist4)));
          pGraphics->GetControlWithTag(kCtrlTagTitle5)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist5)));
          pGraphics->GetControlWithTag(kCtrlTagTitle6)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist6)));
          pGraphics->GetControlWithTag(kCtrlTagTitle7)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist7)));
          pGraphics->GetControlWithTag(kCtrlTagTitle8)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist8)));
          pGraphics->GetControlWithTag(kCtrlTagTitle9)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist9)));
          pGraphics->GetControlWithTag(kCtrlTagTitle10)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist10)));
          pGraphics->GetControlWithTag(kCtrlTagTitle11)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist11)));
          pGraphics->GetControlWithTag(kCtrlTagTitle12)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist12)));
          pGraphics->GetControlWithTag(kCtrlTagTitle13)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist13)));
          //pGraphics->GetControlWithTag(kCtrlTagTitle14)->SetTargetAndDrawRECTs(titleBounds.GetHShifted((float)(dist14)));
          
          pGraphics->GetControlWithTag(kCtrlText)->SetTargetAndDrawRECTs(textBounds);
          pGraphics->GetControlWithTag(kCtrlText2)->SetTargetAndDrawRECTs(textBounds.GetVShifted(10));
        return;
      }

        pGraphics->SetLayoutOnResize(true);
        pGraphics->AttachCornerResizer(EUIResizerMode::Size, true);
        pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
        pGraphics->LoadFont("Apple", APPLE_FN);
        pGraphics->AttachPanelBackground(COLOR_LIGHT_GRAY);
      //pGraphics->AttachControl(new IVSliderControl(IRECT(), kParamGain), kCtrlTagSlider1);
        
//        pGraphics->AttachControl(new IVButtonControl(b.GetCentredInside(100), [=](IControl* pControl) {
//                          // Define the action when the button is clicked
//                          WDL_String file;
//                          pGraphics->PromptForFile(file, EFileAction::Open, "wav", "audio files");
//        
//                          if (file.GetLength()) {
//                              // Handle the file here
//                              // For example, load the audio file or something similar
//                          }
//                      }, "Open File"));
        
        
        pGraphics->AttachControl(new IVNumberBoxControl(IRECT(), kParamDurMult), kCtrlTagSlider2);
      
        pGraphics->AttachControl(new IVSwitchControl(IRECT(), kParamExtreme), kCtrlTagSlider3, "vcontrols");
        //ICaptionControl(<#const IRECT &bounds#>, <#int paramIdx#>)
        GetParam(kParamExtreme)->InitEnum("Mode", 0, 4, "", IParam::kFlagsNone, "", "norm", "e-1", "e-2", "e-3");
        
        //pGraphics->AttachControl(new IVNumberBoxControl(IRECT(), kParamExtreme), kCtrlTagSlider3);
        
        
        //pGraphics->AttachControl(new IVButtonControl(IRECT(), kParamExtreme), kCtrlTagSlider3);
        
//        pGraphics->AttachControl(new IVButtonControl(IRECT(), [=](IControl* pControl) {
//                          // Define the action when the button is clicked
//                          WDL_String loc_file;
//                          //pGraphics->PromptForFile(loc_file, EFileAction::Open, "wav", "audio files");
//
////                          if (file.GetLength()) {
////                              // Handle the file here
////                              // For example, load the audio file or something similar
////                          }
//                      }, "Open File"));
        
        //IText textProps(24, "Arial", IColor(255, 0, 0, 0));
        
//        char myCharArray[13] = "ness_stretch";
//        IColor colors[13] = {IColor(255, 255, 0, 0),
//            IColor(255, 255, 165, 0),
//            IColor(255, 255, 255, 0),
//            IColor(255, 0, 128, 0),
//            IColor(255, 0, 0, 255),
//            IColor(255, 75, 0, 130),
//            IColor(255, 148, 0, 211),
//            IColor(255, 75, 0, 130),
//            IColor(255, 0, 0, 255),
//            IColor(255, 0, 128, 0),
//            IColor(255, 255, 255, 0),
//            IColor(255, 255, 165, 0),
//            IColor(255, 255, 0, 0),
//        };
        
//        for (int i=0; i<12; i++)
//            pGraphics->AttachControl(new ITextControl(IRECT(), "n"/*std::string(1,myCharArray[i])*/, IText(30, colors[i], "Apple",  EAlign::Near)), kCtrlTagTitle1);
      pGraphics->AttachControl(new ITextControl(IRECT(), "n", IText(30, IColor(255, 255, 0, 0), "Apple",  EAlign::Near)), kCtrlTagTitle1);
        pGraphics->AttachControl(new ITextControl(IRECT(), "e", IText(30, IColor(255, 255, 165, 0), "Apple",  EAlign::Near)), kCtrlTagTitle2);
        pGraphics->AttachControl(new ITextControl(IRECT(), "s", IText(30, IColor(255, 255, 255, 0), "Apple",  EAlign::Near)), kCtrlTagTitle3);
        pGraphics->AttachControl(new ITextControl(IRECT(), "s", IText(30, IColor(255, 0, 128, 0), "Apple",  EAlign::Near)), kCtrlTagTitle4);
        pGraphics->AttachControl(new ITextControl(IRECT(), "_", IText(30, IColor(255, 0, 0, 255), "Apple",  EAlign::Near)), kCtrlTagTitle5);
        pGraphics->AttachControl(new ITextControl(IRECT(), "s", IText(30, IColor(255, 75, 0, 255), "Apple",  EAlign::Near)), kCtrlTagTitle6);
        pGraphics->AttachControl(new ITextControl(IRECT(), "t", IText(30, IColor(255, 148, 0, 211), "Apple",  EAlign::Near)), kCtrlTagTitle7);
        pGraphics->AttachControl(new ITextControl(IRECT(), "r", IText(30, IColor(255, 75, 0, 255), "Apple",  EAlign::Near)), kCtrlTagTitle8);
        pGraphics->AttachControl(new ITextControl(IRECT(), "e", IText(30, IColor(255, 0, 0, 255), "Apple",  EAlign::Near)), kCtrlTagTitle9);
        pGraphics->AttachControl(new ITextControl(IRECT(), "t", IText(30, IColor(255, 0, 128, 0), "Apple",  EAlign::Near)), kCtrlTagTitle10);
        pGraphics->AttachControl(new ITextControl(IRECT(), "c", IText(30, IColor(255, 255, 255, 0), "Apple",  EAlign::Near)), kCtrlTagTitle11);
        pGraphics->AttachControl(new ITextControl(IRECT(), "h", IText(30, IColor(255, 255, 165, 0), "Apple",  EAlign::Near)), kCtrlTagTitle12);
        pGraphics->AttachControl(new ITextControl(IRECT(), "1.0", IText(30, IColor(255, 255, 0, 0), "Apple",  EAlign::Near)), kCtrlTagTitle13);
        
        pGraphics->AttachControl(new ITextControl(IRECT(), "duration multplier", IText(20, IColor(255, 255, 165, 0), "Apple",  EAlign::Near)), kSliderLabel2);
        pGraphics->AttachControl(new ITextControl(IRECT(), "stretch algorithm", IText(20, IColor(255, 148, 0, 211), "Apple",  EAlign::Near)), kSliderLabel3);
        
      WDL_String buildInfoStr;
      GetBuildInfoStr(buildInfoStr, __DATE__, __TIME__);
      pGraphics->AttachControl(new ITextControl(IRECT(), "requires extreme cpu use. clicks will likely be heard in playback.", IText(8, IColor(255, 0, 0, 255), "Apple",  EAlign::Far)), kCtrlText);
        pGraphics->AttachControl(new ITextControl(IRECT(), "render the track for a smooth experience.", IText(8, IColor(255, 0, 128, 0), "Apple",  EAlign::Far)), kCtrlText2);
      
      goto layout;
    

    };
#endif
}


#if IPLUG_EDITOR
void NessStretch::OnParentWindowResize(int width, int height)
{
    if(GetUI())
        GetUI()->Resize(width, height, 1.f, false);
}
#endif

template <typename T, typename U, typename V> inline T clip(T x, U lo, V hi) {
    return std::max(std::min(x, (T)hi), (T)lo);
}

inline float cubicinterp(float x, float y0, float y1, float y2, float y3) {
    // 4-point, 3rd-order Hermite (x-form)
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
    float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
    
    return ((c3 * x + c2) * x + c1) * x + c0;
}

void NessStretch::GetPlaybuf(Buffer<float> inbuf, float* outbuf, int start_frame, int num_samples){
    float rate = m_file_samplerate/m_daw_samplerate;
    for (int i=0; i<num_samples; i++){
        float buf_point = ((float)i*rate)+(float)start_frame;
        
        long bp1 = long(floor(buf_point));
        
        //quadratic interpolation on the copy
        if(bp1<inbuf.getSize()){
            float fracphase = buf_point - floor(buf_point);
            
            long bp0 = clip(bp1-1, 0, inbuf.getSize()-1);
            long bp2 = clip(bp1+1, 0, inbuf.getSize()-1);
            long bp3 = clip(bp1+2, 0, inbuf.getSize()-1);
            
            outbuf[i] = cubicinterp(fracphase, inbuf.data()[bp0], inbuf.data()[bp1], inbuf.data()[bp2], inbuf.data()[bp3]);
            
        } else {
            outbuf[i]=0.f;
        }
    }
}

#if IPLUG_DSP
void NessStretch::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
    const int nChans = NOutChansConnected();
    //const double gain = GetParam(kParamGain)->Value() / 100.;
    //const double gain = 0.1;
    const int dur_mult = (int)GetParam(kParamDurMult)->Value();
    const int extreme = (int)GetParam(kParamExtreme)->Value();
    
    const int daw_samplerate = (int)GetSampleRate();
    
    if(m_file_loaded==true){
    
        //this is super hacky. should just update the object rather than making a new one.
        if((dur_mult!=m_dur_mult)||(extreme!=m_extreme)||((int)m_daw_samplerate!=daw_samplerate)){
            m_dur_mult = dur_mult;
            m_extreme = extreme;
            m_daw_samplerate = GetSampleRate();
            
            int num_slices = (m_daw_samplerate>50000.0) ? 10: 9;
            m_max_win_size = (m_daw_samplerate>50000.0) ? 131072:65536;
            
            for(int c = 0; c<2; c++){
                ness_stretch_free(rusties[c]);
                rusties[c] = ness_stretch_new((float)m_dur_mult, m_max_win_size, 1, num_slices, m_extreme, 8192);
            }
            m_play_counter = m_max_win_size;
        }
        
        m_daw_samplerate = GetSampleRate();
        double samplePosition = GetSamplePos()*m_file_samplerate/m_daw_samplerate/m_dur_mult;
        
        //if we are at the
        if((m_play_counter>=m_max_win_size)||(m_playing==false&&GetTransportIsRunning())) {
            m_playing = true;
            m_play_counter = 0;
            m_file.seek((int)samplePosition);
            
            for (int c = 0; c < nChans; c++) {
                GetPlaybuf(m_buffers[c], m_process_buffers[c], (int)samplePosition, m_max_win_size);
            }
            
            for (int i = 0; i<m_max_win_size; i++)
            {
                for(int c = 0; c<2; c++)
                    ness_stretch_set_in_chunk(rusties[c], m_process_buffers[c][i], i);
            }
            //do the ness_stretch
            for(int c = 0; c<2; c++){
                ness_stretch_calc(rusties[c]);
                ness_stretch_move_in_to_stored(rusties[c]);
            }
            
        }
        
        if ((m_play_counter>=0) && (GetTransportIsRunning())) {
            for(int c = 0; c<2; c++){
                ness_stretch_next(rusties[c], m_process_buffers[c], nFrames, m_play_counter);
                for (int s = 0; s<nFrames; s++){
                    outputs[c][s]=m_process_buffers[c][s];
                }
            }
            
        } else {
            m_playing = false;
            for (int i = 0; i<nFrames; i++){
                outputs[0][i]=0.0;
                outputs[1][i]=0.0;
            }
        }

        m_play_counter += nFrames;
    
    } else {
        for (int i = 0; i<nFrames; i++){
            outputs[0][i]=0.0;
            outputs[1][i]=0.0;
        }
    }
}
#endif
