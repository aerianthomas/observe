#import "TrackingResonatorBankCpp.h"

#import <Foundation/Foundation.h>

#include "TrackingResonatorBank.hpp"

using namespace oscillators_cpp;

@interface TrackingResonatorBankCpp()
@property oscillators_cpp::TrackingResonatorBank *resonatorBank;
@end

@implementation TrackingResonatorBankCpp

- (instancetype)initWithNumResonators:(int)numResonators naturalFrequencies:(const float*)naturalFrequencies alphas:(const float*)alphas betas: (const float*)betas gammas: (const float*)gammas sampleRate:(float)sampleRate {
    if (self = [super init]) {
        self.resonatorBank = new TrackingResonatorBank(numResonators, naturalFrequencies, alphas, betas, gammas, sampleRate);
    }
    return self;
}

- (void)dealloc {
    delete self.resonatorBank;
}

- (float)sampleRate {
    return self.resonatorBank->sampleRate();
}

- (int)numResonators {
    return static_cast<int>(self.resonatorBank->numResonators());
}

- (void)getNaturalFrequencies:(float*)dest size: (int)size {
    self.resonatorBank->getNaturalFrequencies(dest, size);
}

- (void)getResonantFrequencies:(float*)dest size: (int)size {
    self.resonatorBank->getResonantFrequencies(dest, size);
}

- (float)resonantFrequencyValue:(int)index {
    return self.resonatorBank->resonantFrequencyValue(index);
}

- (void)getPowers:(float*)dest size: (int)size {
    self.resonatorBank->getPowers(dest, size);
}

- (void)getAmplitudes:(float*)dest size: (int)size {
    self.resonatorBank->getAmplitudes(dest, size);
}

- (void)getPhases:(float*)dest size: (int)size {
    self.resonatorBank->getPhases(dest, size);
}

- (void)getDeltaPhases:(float*)dest size: (int)size {
    self.resonatorBank->getDeltaPhases(dest, size);
}

- (float)accPower {
    return self.resonatorBank->accPower();
}

- (void)update:(float)sample {
    self.resonatorBank->update(sample);
}

- (void)update:(float*)frame frameLength:(int)frameLength sampleStride:(int)sampleStride {
    self.resonatorBank->update(frame, frameLength, sampleStride);
}

- (void)updateConcurrent:(float*)frame frameLength:(int)frameLength sampleStride:(int)sampleStride {
    self.resonatorBank->updateConcurrent(frame, frameLength, sampleStride);
}

- (void)setTimeConstant:(float)tau frameLength:(int)frameLength sampleStride:(int)sampleStride sampleRate:(float)sampleRate {
    self.resonatorBank->setTimeConstant(tau, frameLength, sampleStride, sampleRate);
}

@end
