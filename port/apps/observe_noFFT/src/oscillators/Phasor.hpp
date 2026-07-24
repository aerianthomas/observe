#ifndef Phasor_hpp
#define Phasor_hpp

#include <cmath>
#include <vector>

namespace oscillators_cpp
{

    constexpr float PI = 3.14159265358979323846; // PI
    constexpr float twoPi = 2.0 * PI;

    // Phasor class: base for individual oscillators
    class Phasor
    {

    private:
        float m_omega; // must be set trhough accessor

    protected:
        float m_sampleRate;

        // Phasor
        float m_Zc;
        float m_Zs;
        float m_Wc;
        float m_Ws;
        float m_Wcps;

        void updateMultiplier();

    public:
        Phasor(float frequency, float sampleRate, bool angular = false);

        float omega() const { return m_omega; }
        void setOmega(float omega);
        float frequency() const { return -m_sampleRate * m_omega / twoPi; }
        void setFrequency(float frequency);
        float sampleRate() const { return m_sampleRate; }
        void setSampleRate(float sampleRate);

        float Zc() const { return m_Zc; }
        float Zs() const { return m_Zs; }

        void incrementPhase();
        void stabilize();
    };

} // oscillators_cpp

#endif /* Phasor_hpp */
