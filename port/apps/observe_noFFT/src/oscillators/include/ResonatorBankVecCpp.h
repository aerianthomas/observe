#import <Foundation/Foundation.h>

// Wrapper for the ResonatorBank class
@interface ResonatorBankVecCpp : NSObject
- (instancetype)initWithNumResonators:(int)numResonators frequencies:(const float *)frequencies alphas:(const float *)alphas betas:(const float *)betas sampleRate:(float)sampleRate;
- (float)sampleRate;
- (int)numResonators;
- (float)frequencyValue:(int)index;
- (float)alphaValue:(int)index;
- (float)betaValue:(int)index;
- (void)getPowers:(float *)dest size:(int)size;
- (void)getAmplitudes:(float *)dest size:(int)size;
- (void)getPhases:(float *)dest size:(int)size;
- (void)update:(float)sample
    NS_SWIFT_NAME(update(sample:));
- (void)update:(float *)frame frameLength:(int)frameLength sampleStride:(int)sampleStride
    NS_SWIFT_NAME(update(frameData:frameLength:sampleStride:));
- (void)update:(float *)frame frameLength:(int)frameLength sampleStride:(int)sampleStride powers:(float *)powers amplitudes:(float *)amplitudes
    NS_SWIFT_NAME(update(frameData:frameLength:sampleStride:powers:amplitudes:));
@end
