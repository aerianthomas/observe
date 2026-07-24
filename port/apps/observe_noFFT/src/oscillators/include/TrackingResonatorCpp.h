#import <Foundation/Foundation.h>
#import "PhasorCpp.h"

// Wrapper for the TrackingResonator class
@interface TrackingResonatorCpp : PhasorCpp
- (instancetype)initWithNaturalFrequency:(float)naturalFrequency alpha:(float)alpha beta:(float)beta gamma:(float)gamma sampleRate:(float)sampleRate;
- (float)naturalFrequency;
- (void)setNaturalFrequency:(float)frequency alpha:(float)alpha beta:(float)beta gamma:(float)gamma;
- (float)resonantFrequency;
- (float)power;
- (float)amplitude;
- (float)alpha;
- (void)setAlpha:(float)alpha;
- (float)omAlpha; // used in test...
- (float)beta;
- (void)setBeta:(float)beta;
- (float)omBeta; // used in test...
- (float)gamma;
- (void)setGamma:(float)gamma;
- (float)omGamma; // used in test...
- (float)c;
- (float)s;
- (float)cc;
- (float)ss;
- (float)dpc;
- (float)dps;
- (float)phase;
- (float)deltaPhase;
- (void)updateWithSample:(float)sample
    NS_SWIFT_NAME(updateWithSample(value:));
- (void)update:(float)sample maxPower:(float)maxPower
    NS_SWIFT_NAME(update(sample:maxPower:));
- (void)update:(float *)frame frameLength:(int)frameLength sampleStride:(int)sampleStride maxPower:(float)maxPower
    NS_SWIFT_NAME(update(frameData:frameLength:sampleStride:maxPower:));
@end
