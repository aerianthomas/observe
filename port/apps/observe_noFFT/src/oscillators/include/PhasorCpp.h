#import <Foundation/Foundation.h>

// Wrapper for the base Phasor class
@interface PhasorCpp : NSObject
- (instancetype)initWithFrequency:(float)frequency sampleRate:(float)sampleRate angular:(bool)angular;
- (float)frequency;
- (void)setFrequency:(float)frequency;
- (float)sampleRate;

@end
