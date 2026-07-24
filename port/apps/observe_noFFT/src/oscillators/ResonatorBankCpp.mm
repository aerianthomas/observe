#import "ResonatorBankCpp.h"

#import <Foundation/Foundation.h>

#include "ResonatorBank.hpp"

using namespace oscillators_cpp;

@interface ResonatorBankCpp()
@property oscillators_cpp::ResonatorBank *resonatorBank;
@end

@implementation ResonatorBankCpp

- (instancetype)initWithNumResonators:(int)numResonators frequencies:(const float*)frequencies alphas:(const float*)alphas betas: (const float*)betas gammas: (const float*)gammas sampleRate:(float)sampleRate {
    if (self = [super init]) {
        self.resonatorBank = new ResonatorBank(numResonators, frequencies, alphas, betas, gammas, sampleRate);
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

- (float)frequencyValue:(int)index {
    return self.resonatorBank->frequencyValue(index);
}

- (float)alphaValue:(int)index {
    return self.resonatorBank->alphaValue(index);
}

- (void)setAllAlphas:(float)alpha {
    self.resonatorBank->setAllAlphas(alpha);
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

- (void)getInstantaneousFrequencies:(float*)dest size: (int)size {
    self.resonatorBank->getInstantaneousFrequencies(dest, size);
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

@end
