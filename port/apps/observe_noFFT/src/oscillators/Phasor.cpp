#include "Phasor.hpp"

#include <cmath>
#include <iostream>

#if defined(__APPLE__)
#include <Accelerate/Accelerate.h>
#endif

using namespace oscillators_cpp;

Phasor::Phasor(float frequency, float sampleRate, bool angular)
    : m_omega(angular ? frequency : -twoPi * frequency / sampleRate), m_sampleRate(sampleRate),
      m_Zc(1.0), m_Zs(0.0)
{
    updateMultiplier();
}

void Phasor::updateMultiplier()
{
    m_Wc = cos(m_omega);
    m_Ws = sin(m_omega);
    m_Wcps = m_Wc + m_Ws;
}

void Phasor::setOmega(float omega)
{
    m_omega = omega;
    updateMultiplier();
}

void Phasor::setFrequency(float frequency)
{
    m_omega = -twoPi * frequency / m_sampleRate;
    updateMultiplier();
}

void Phasor::setSampleRate(float sampleRate)
{
    m_sampleRate = sampleRate;
    updateMultiplier();
}

void Phasor::incrementPhase()
{
    // complex multiplication with 3 real multiplications
    const float ac = m_Wc * m_Zc;
    const float bd = m_Ws * m_Zs;
    const float abcd = m_Wcps * (m_Zc + m_Zs);
    m_Zc = ac - bd;
    m_Zs = abcd - ac - bd;
}

void Phasor::stabilize()
{
    // approximation for 1 / sqrt(x) around 1 (Taylor expansion)
    // sqrt(m_Zc*m_Zc + m_Zs*m_Zs) should be 1
    const float k = (3.0f - m_Zc * m_Zc - m_Zs * m_Zs) / 2.0f;
    m_Zc *= k;
    m_Zs *= k;
}
