#import <Foundation/Foundation.h>

#import "PhasorCpp.h"
#import "PhasorCppProtected.h"

using namespace oscillators_cpp;

@implementation PhasorCpp

- (instancetype)initWithFrequency:(float)frequency sampleRate:(float)sampleRate angular:(bool)angular {
    if (self = [super init]) {
        self.oscillator = new Phasor(frequency, sampleRate, angular);
    }
    return self;
}

- (void)dealloc {
    delete self.oscillator;
}

- (float)frequency {
    return self.oscillator->frequency();
}

- (void)setFrequency:(float)frequency {
    self.oscillator->setFrequency(frequency);
}

- (float)sampleRate {
    return self.oscillator->sampleRate();
}

@end
