#import "ResonatorBankVecCpp.h"

#import <Foundation/Foundation.h>

#include "ResonatorBankVec.hpp"

using namespace oscillators_cpp;

@interface ResonatorBankVecCpp()
@property oscillators_cpp::ResonatorBankVec *resonatorBank;
@end

@implementation ResonatorBankVecCpp

- (instancetype)initWithNumResonators:(int)numResonators frequencies:(const float*)frequencies alphas:(const float*)alphas betas:(const float*)betas sampleRate:(float)sampleRate {
    if (self = [super init]) {
        self.resonatorBank = new ResonatorBankVec(numResonators, frequencies, alphas, betas, sampleRate);
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

- (float)betaValue:(int)index {
    return self.resonatorBank->betaValue(index);
}

- (void)getPowers:(float*)dest size: (int)size {
    self.resonatorBank->getPowers(dest, size);
}

//- (float)powerValue:(int)index {
//    return self.resonatorBank->powerValue(index);
//}

- (void)getAmplitudes:(float*)dest size: (int)size {
    self.resonatorBank->getAmplitudes(dest, size);
}

//- (float)amplitudeValue:(int)index {
//    return self.resonatorBank->amplitudeValue(index);
//}

- (void)getPhases:(float*)dest size: (int)size {
    self.resonatorBank->getPhases(dest, size);
}

- (void)update:(float)sample {
    self.resonatorBank->update(sample);
}

- (void)update:(float*)frame frameLength:(int)frameLength sampleStride:(int)sampleStride {
    self.resonatorBank->update(frame, frameLength, sampleStride);
}

- (void)update:(float*)frame frameLength:(int)frameLength sampleStride:(int)sampleStride powers:(float*)powers amplitudes:(float*)amplitudes {
    self.resonatorBank->update(frame, frameLength, sampleStride, powers, amplitudes);
}

@end
