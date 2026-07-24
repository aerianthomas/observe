#import "ResonatorCpp.h"
#import "PhasorCppProtected.h"

#import <Foundation/Foundation.h>

#include "Resonator.hpp"

using namespace oscillators_cpp;

@implementation ResonatorCpp

- (instancetype)initWithFrequency:(float)frequency alpha:(float)alpha beta:(float)beta gamma:(float)gamma sampleRate:(float)sampleRate {
    if (self = [super init]) {
        self.oscillator = new Resonator(frequency, alpha, beta, gamma, sampleRate);
    }
    return self;
}

- (Resonator*)resonator {
    return (Resonator*)self.oscillator;
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

- (float)instantaneousFrequency {
    return self.resonator->instantaneousFrequency();
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

- (float)phase {
    return self.resonator->phase();
}

- (float)dpc {
    return self.resonator->dpc();
}

- (float)dps {
    return self.resonator->dps();
}

- (float)deltaPhase {
    return self.resonator->deltaPhase();
}

- (void)updateWithSample:(float)sample {
    self.resonator->updateWithSample(sample);
}

- (void)update:(float)sample {
    self.resonator->update(sample);
}

- (void)update:(float*)frame frameLength:(int)frameLength sampleStride:(int)sampleStride {
    self.resonator->update(frame, frameLength, sampleStride);
}

@end
