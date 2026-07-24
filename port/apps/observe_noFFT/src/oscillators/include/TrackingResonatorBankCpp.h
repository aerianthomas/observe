#import <Foundation/Foundation.h>

// Wrapper for the TrackingResonatorBank class
@interface TrackingResonatorBankCpp : NSObject
- (instancetype)initWithNumResonators:(int)numResonators naturalFrequencies:(const float *)frequencies alphas:(const float *)alphas betas:(const float *)betas gammas:(const float *)gammas sampleRate:(float)sampleRate;
- (float)sampleRate;
- (int)numResonators;
- (void)getNaturalFrequencies:(float *)dest size:(int)size;
- (void)getResonantFrequencies:(float *)dest size:(int)size;
- (float)resonantFrequencyValue:(int)index;
- (void)getPowers:(float *)dest size:(int)size;
- (void)getAmplitudes:(float *)dest size:(int)size;
- (void)getPhases:(float *)dest size:(int)size;
- (void)getDeltaPhases:(float *)dest size:(int)size;
- (float)accPower;
- (void)update:(float)sample
    NS_SWIFT_NAME(update(sample:));
- (void)update:(float *)frame frameLength:(int)frameLength sampleStride:(int)sampleStride
    NS_SWIFT_NAME(update(frameData:frameLength:sampleStride:));
- (void)updateConcurrent:(float *)frame frameLength:(int)frameLength sampleStride:(int)sampleStride
    NS_SWIFT_NAME(updateConcurrent(frameData:frameLength:sampleStride:));
- (void)setTimeConstant:(float)tau frameLength:(int)frameLength sampleStride:(int)sampleStride sampleRate:(float)sampleRate;
@end
