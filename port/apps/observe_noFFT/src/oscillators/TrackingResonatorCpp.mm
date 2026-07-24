#import "TrackingResonatorCpp.h"
#import "PhasorCppProtected.h"

#import <Foundation/Foundation.h>

#include "TrackingResonator.hpp"

using namespace oscillators_cpp;

@implementation TrackingResonatorCpp

- (instancetype)initWithNaturalFrequency:(float)naturalFrequency alpha:(float)alpha beta:(float)beta gamma:(float)gamma sampleRate:(float)sampleRate {
    if (self = [super init]) {
        self.oscillator = new TrackingResonator(naturalFrequency, alpha, beta, gamma, sampleRate);
    }
    return self;
}

- (TrackingResonator*)resonator {
    return (TrackingResonator*)self.oscillator;
}

- (float)naturalFrequency {
    return self.resonator->naturalFrequency();
}

- (void)setNaturalFrequency:(float)frequency alpha:(float)alpha beta:(float)beta gamma:(float)gamma {
    return self.resonator->setNaturalFrequency(frequency, alpha, beta, gamma);
}

- (float)resonantFrequency {
    return self.resonator->resonantFrequency();
}

- (float)power {
    return self.resonator->power();
}

- (float)amplitude {
    return self.resonator->amplitude();
}

- (float)alpha {
    return self.resonator->alpha();
}

- (void)setAlpha:(float)alpha {
    self.resonator->setAlpha(alpha);
}

- (float)omAlpha {
    return self.resonator->omAlpha();
}

- (float)beta {
    return self.resonator->beta();
}

- (void)setBeta:(float)beta {
    self.resonator->setBeta(beta);
}

- (float)omBeta {
    return self.resonator->omBeta();
}

- (float)gamma {
    return self.resonator->gamma();
}

- (void)setGamma:(float)gamma {
    self.resonator->setGamma(gamma);
}

- (float)omGamma {
    return self.resonator->omGamma();
}

- (float)c {
    return self.resonator->c();
}

- (float)s {
    return self.resonator->s();
}

- (float)cc {
    return self.resonator->cc();
}

- (float)ss {
    return self.resonator->ss();
}

- (float)dpc {
    return self.resonator->dpc();
}

- (float)dps {
    return self.resonator->dps();
}

- (float)phase {
    return self.resonator->phase();
}

- (float)deltaPhase {
    return self.resonator->deltaPhase();
}

- (void)updateWithSample:(float)sample {
    self.resonator->updateWithSample(sample);
}

- (void)update:(float)sample maxPower:(float)maxPower {
    self.resonator->update(sample, maxPower);
}

- (void)update:(float*)frame frameLength:(int)frameLength sampleStride:(int)sampleStride maxPower:(float)maxPower {
    self.resonator->update(frame, frameLength, sampleStride, maxPower);
}

@end
