
#include <TargetConditionals.h>
#if TARGET_OS_IOS == 1
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#define IPLUG_AUVIEWCONTROLLER IPlugAUViewController_vNessStretch
#define IPLUG_AUAUDIOUNIT IPlugAUAudioUnit_vNessStretch
#import <NessStretchAU/IPlugAUViewController.h>
#import <NessStretchAU/IPlugAUAudioUnit.h>

//! Project version number for NessStretchAU.
FOUNDATION_EXPORT double NessStretchAUVersionNumber;

//! Project version string for NessStretchAU.
FOUNDATION_EXPORT const unsigned char NessStretchAUVersionString[];

@class IPlugAUViewController_vNessStretch;
