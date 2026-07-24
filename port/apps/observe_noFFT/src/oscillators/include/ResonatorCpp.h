#import <Foundation/Foundation.h>
#import "PhasorCpp.h"

// Wrapper for the Resonator class
@interface ResonatorCpp : PhasorCpp
- (instancetype)initWithFrequency:(float)frequency alpha:(float)alpha beta:(float)beta gamma:(float)gamma sampleRate:(float)sampleRate;
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
- (float)instantaneousFrequency;
- (void)updateWithSample:(float)sample
    NS_SWIFT_NAME(updateWithSample(value:));
- (void)update:(float)sample
    NS_SWIFT_NAME(update(sample:));
- (void)update:(float *)frame frameLength:(int)frameLength sampleStride:(int)sampleStride
    NS_SWIFT_NAME(update(frameData:frameLength:sampleStride:));
@end
