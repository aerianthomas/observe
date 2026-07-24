#include "Oscillator.hpp"

using namespace oscillators_cpp;

Oscillator::Oscillator(float frequency, float sampleRate, float amplitude, bool angular)
    : Phasor(frequency, sampleRate, angular), m_amplitude(amplitude)
{
}
