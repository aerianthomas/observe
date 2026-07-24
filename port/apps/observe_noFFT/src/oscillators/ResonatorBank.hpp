#ifndef ResonatorBank_hpp
#define ResonatorBank_hpp

#include "Resonator.hpp"

#include <vector>

// use GCD concurrency by default
// uncomment the next line to use std::async instead
// #define STD_CONCURRENCY

#ifndef STD_CONCURRENCY
#include <dispatch/dispatch.h>
#endif

namespace oscillators_cpp
{

    class ResonatorBank
    {
    private:
        float m_sampleRate;
        std::vector<std::unique_ptr<Resonator>> m_resonators;

#ifndef STD_CONCURRENCY
        dispatch_group_t m_dispatchGroup;
        dispatch_queue_t m_dispatchQueue;
#endif

    public:
        ResonatorBank &operator=(const ResonatorBank &) = delete;
        ResonatorBank(const ResonatorBank &) = delete;

        ResonatorBank(size_t numResonators, const float *frequencies, const float *alphas, const float *betas, const float *gammas, float sampleRate);
#ifndef STD_CONCURRENCY
        ~ResonatorBank();
#endif

        float sampleRate() { return m_sampleRate; }
        size_t numResonators() { return m_resonators.size(); }
        float frequencyValue(size_t index);
        float alphaValue(size_t index);
        void setAllAlphas(float alpha);
        void getPowers(float *dest, size_t size);
        void getAmplitudes(float *dest, size_t size);
        void getPhases(float *dest, size_t size);
        void getDeltaPhases(float *dest, size_t size);
        void getInstantaneousFrequencies(float *dest, size_t size);
        void update(const float sample);
        void update(const std::vector<float> &samples);
        void update(const float *frameData, size_t frameLength, size_t sampleStride);
        void updateConcurrent(const float *frameData, size_t frameLength, size_t sampleStride);

#ifdef STD_CONCURRENCY
    private:
        void updateEvery(size_t mod, size_t offset, const float *frameData, size_t frameLength, size_t sampleStride);
#endif
    };

} // oscillators_cpp

#endif /* ResonatorBank_hpp */
