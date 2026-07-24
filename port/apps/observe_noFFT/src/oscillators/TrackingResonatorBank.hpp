#ifndef TrackingResonatorBank_hpp
#define TrackingResonatorBank_hpp

#include "TrackingResonator.hpp"

#include <vector>
#include <dispatch/dispatch.h>

namespace oscillators_cpp
{

    class TrackingResonatorBank
    {
    private:
        float m_sampleRate;
        std::vector<std::unique_ptr<TrackingResonator>> m_resonators;

        // max power accumulation
        float m_sigma;
        float m_omSigma;
        std::atomic<float> m_accPower;

        dispatch_queue_t m_dispatchQueue;

    public:
        TrackingResonatorBank &operator=(const TrackingResonatorBank &) = delete;
        TrackingResonatorBank(const TrackingResonatorBank &) = delete;

        TrackingResonatorBank(size_t numResonators, const float *naturalFrequencies, const float *alphas, const float *betas, const float *gammas, float sampleRate);

        float sampleRate() { return m_sampleRate; }
        size_t numResonators() { return m_resonators.size(); }
        void getNaturalFrequencies(float *dest, size_t size);
        void getResonantFrequencies(float *dest, size_t size);
        float resonantFrequencyValue(size_t index);
        void getPowers(float *dest, size_t size);
        void getAmplitudes(float *dest, size_t size);
        void getPhases(float *dest, size_t size);
        void getDeltaPhases(float *dest, size_t size);
        float accPower() const { return m_accPower; };
        void update(const float sample);
        void update(const std::vector<float> &samples);
        void update(const float *frameData, size_t frameLength, size_t sampleStride);
        void updateConcurrent(const float *frameData, size_t frameLength, size_t sampleStride);
        void setTimeConstant(float tau, size_t frameLength, size_t sampleStride, float sampleRate);
    };

} // oscillators_cpp

#endif /* TrackingResonatorBank_hpp */
