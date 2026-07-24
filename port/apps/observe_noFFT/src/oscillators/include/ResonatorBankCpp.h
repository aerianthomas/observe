#import <Foundation/Foundation.h>

// Wrapper for the ResonatorBank class
@interface ResonatorBankCpp : NSObject
- (instancetype)initWithNumResonators:(int)numResonators frequencies:(const float *)frequencies alphas:(const float *)alphas betas:(const float *)betas gammas:(const float *)gammas sampleRate:(float)sampleRate;
- (float)sampleRate;
- (int)numResonators;
- (float)frequencyValue:(int)index;
- (float)alphaValue:(int)index;
- (void)setAllAlphas:(float)alpha;
- (void)getPowers:(float *)dest size:(int)size;
- (void)getAmplitudes:(float *)dest size:(int)size;
- (void)getPhases:(float *)dest size:(int)size;
- (void)getDeltaPhases:(float *)dest size:(int)size;
- (void)getInstantaneousFrequencies:(float *)dest size:(int)size;
- (void)update:(float)sample
    NS_SWIFT_NAME(update(sample:));
- (void)update:(float *)frame frameLength:(int)frameLength sampleStride:(int)sampleStride
    NS_SWIFT_NAME(update(frameData:frameLength:sampleStride:));
- (void)updateConcurrent:(float *)frame frameLength:(int)frameLength sampleStride:(int)sampleStride
    NS_SWIFT_NAME(updateConcurrent(frameData:frameLength:sampleStride:));
@end
