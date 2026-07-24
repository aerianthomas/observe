#ifndef Oscillator_hpp
#define Oscillator_hpp

#include "Phasor.hpp"

#include <vector>

namespace oscillators_cpp
{

    // Oscillator class: simple signal generator
    class Oscillator : public Phasor
    {
    protected:
        float m_amplitude;

    public:
        Oscillator(float frequency, float sampleRate, float amplitude = 1.0f, bool angular = false);

        float amplitude() const { return m_amplitude; }
        void setAmplitude(float amplitude) { m_amplitude = amplitude; }
        float power() const { return m_amplitude * m_amplitude; }

        float sample() const { return m_amplitude * m_Zc; }
    };

} // oscillators_cpp

#endif /* Oscillator_hpp */
