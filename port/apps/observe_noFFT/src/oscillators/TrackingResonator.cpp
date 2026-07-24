#include "TrackingResonator.hpp"

#if defined(__APPLE__)
#include <Accelerate/Accelerate.h>
#endif

#include <cmath>
#include <stdexcept>

using namespace oscillators_cpp;

constexpr float minMaxPower = 0.001;

TrackingResonator::TrackingResonator(float naturalFrequency, float alpha, float beta, float gamma, float sampleRate) : Phasor(naturalFrequency, sampleRate),
                                                                                                                       m_naturalFrequency(naturalFrequency), m_alpha(alpha), m_omAlpha(1.0 - alpha), m_beta(beta), m_omBeta(1.0 - beta), m_gamma(gamma), m_omGamma(1.0 - gamma),
                                                                                                                       m_dpc(1.0), m_dps(0.0), m_trackFrequencyPowerThreshold(0.001)
{
}

void TrackingResonator::setNaturalFrequency(float frequency, float alpha, float beta, float gamma)
{
    m_naturalFrequency = frequency;
    setAlpha(alpha);
    setBeta(beta);
    setGamma(gamma);
}

void TrackingResonator::setAlpha(float alpha)
{
    if (alpha < 0.0 || alpha > 1.0)
    {
        throw std::out_of_range("Bad alpha passed to setAlpha()");
    }
    m_alpha = alpha;
    m_omAlpha = 1.0 - m_alpha;
}

void TrackingResonator::setBeta(float beta)
{
    if (beta < 0.0 || beta > 1.0)
    {
        throw std::out_of_range("Bad beta passed to setBeta()");
    }
    m_beta = beta;
    m_omBeta = 1.0 - m_beta;
}

void TrackingResonator::setGamma(float gamma)
{
    if (gamma < 0.0 || gamma > 1.0)
    {
        throw std::out_of_range("Bad gamma passed to setGamma()");
    }
    m_gamma = gamma;
    m_omGamma = 1.0 - m_gamma;
}

float TrackingResonator::phase() const
{
    return atan2(m_ss, m_cc); // returns value in [-pi,pi]
}

float TrackingResonator::deltaPhase() const
{
    return atan2(m_dps, m_dpc); // returns value in [-pi,pi]
}

void TrackingResonator::updateWithSample(float sample)
{
    const float alphaSample = m_alpha * sample;
    m_cos = m_omAlpha * m_cos + alphaSample * m_Zc;
    m_sin = m_omAlpha * m_sin + alphaSample * m_Zs;
    const float lcc = m_cc;
    const float lss = m_ss;
    m_cc = m_omBeta * m_cc + m_beta * m_cos;
    m_ss = m_omBeta * m_ss + m_beta * m_sin;
    // compute current * conjugate(previous)
    // the phase time derivative estimate is the arg of this complex number
    m_dpc = m_cc * lcc + m_ss * lss;
    m_dps = m_ss * lcc - m_cc * lss;

    // Update tracking
    if (power() > m_trackFrequencyPowerThreshold)
    {
        // go towards instantaneous frequency
        // this is EWMA with parameter gamma
        setOmega(omega() - m_gamma * atan2(m_dps, m_dpc));
    }
    else
    {
        // go back to natural frequency
        setFrequency(m_naturalFrequency);
    }

    incrementPhase();
}

void TrackingResonator::update(float sample, float maxPower)
{
    m_trackFrequencyPowerThreshold = fmax(minMaxPower, maxPower) / 1000.0f;
    updateWithSample(sample);
    stabilize(); // this is overkill but necessary
}

void TrackingResonator::update(const std::vector<float> &samples, float maxPower)
{
    m_trackFrequencyPowerThreshold = fmax(minMaxPower, maxPower) / 1000.0f;
    for (float sample : samples)
    {
        updateWithSample(sample);
    }
    stabilize(); // this is overkill but necessary
}

void TrackingResonator::update(const float *frameData, size_t frameLength, size_t sampleStride, float maxPower)
{
    m_trackFrequencyPowerThreshold = fmax(minMaxPower, maxPower) / 1000.0f;
    for (int i = 0; i < frameLength; i += sampleStride)
    {
        updateWithSample(frameData[i]);
    }
    stabilize(); // this is overkill but necessary
}
