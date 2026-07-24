#pragma once

#include <observe/app.h>

#include <mutex>
#include <vector>

#include "Oscillator.hpp"
#include "Resonator.hpp"

namespace oscillators_observe
{

  constexpr double kDefaultSampleRate = 48000.0;
  constexpr int kNumResonators = 84;
  constexpr float kMinFrequency = 55.0f;
  constexpr int kBinsPerOctave = 12;

  class ResonateModel
  {
  public:
    explicit ResonateModel(double sample_rate = kDefaultSampleRate);

    void setSampleRate(double sample_rate);
    double sampleRate() const { return sample_rate_; }

    int size() const { return kNumResonators; }
    float frequency(int i) const { return frequencies_[i]; }

    void processBlock(const float *in, int num_samples);
    void generateSynthetic(int num_samples);
    void snapshot(std::vector<float> &out_amplitudes, float &out_last_sample);

  private:
    static constexpr int kNumVoices = 3;
    static constexpr int kStabilizeInterval = 1024;

    void rebuild();
    float alphaForTimeConstant(float time_constant) const;
    void pushSample(float sample);
    void publish();

    double sample_rate_ = kDefaultSampleRate;
    std::vector<oscillators_cpp::Resonator> resonators_;
    std::vector<oscillators_cpp::Oscillator> voices_;
    std::vector<float> frequencies_;

    std::mutex mutex_;
    std::vector<float> amplitudes_;
    float published_sample_ = 0.0f;

    int retune_counter_ = 0;
    int chord_step_ = 0;
    int stabilize_counter_ = 0;
    float last_sample_ = 0.0f;
  };

  class SpectrumView
  {
  public:
    SpectrumView(observe::ApplicationWindow &app, ResonateModel &model, bool synthesize_when_idle);
    void install();

  private:
    observe::ApplicationWindow &app_;
    ResonateModel &model_;
    bool synthesize_when_idle_ = false;
    std::vector<float> amplitudes_;
    float last_sample_ = 0.0f;
    float display_peak_ = 1.0e-3f;
    double last_time_ = -1.0;
  };

} // namespace oscillators_observe
