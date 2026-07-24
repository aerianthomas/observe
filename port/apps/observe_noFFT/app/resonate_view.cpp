#include "resonate_view.h"

#include <algorithm>
#include <cmath>

namespace oscillators_observe
{

ResonateModel::ResonateModel(double sample_rate)
{
  sample_rate_ = sample_rate > 0.0 ? sample_rate : kDefaultSampleRate;
  rebuild();
}

void ResonateModel::setSampleRate(double sample_rate)
{
  if (sample_rate <= 0.0 || sample_rate == sample_rate_)
    return;

  std::lock_guard<std::mutex> lock(mutex_);
  sample_rate_ = sample_rate;
  rebuild();
}

void ResonateModel::rebuild()
{
  const float alpha = alphaForTimeConstant(0.05f);
  const float beta = alphaForTimeConstant(0.10f);
  const auto sample_rate = static_cast<float>(sample_rate_);

  resonators_.clear();
  voices_.clear();
  frequencies_.clear();
  resonators_.reserve(kNumResonators);
  frequencies_.reserve(kNumResonators);
  amplitudes_.assign(kNumResonators, 0.0f);

  for (int i = 0; i < kNumResonators; ++i)
  {
    const float freq = kMinFrequency * std::pow(2.0f, static_cast<float>(i) / kBinsPerOctave);
    frequencies_.push_back(freq);
    resonators_.emplace_back(freq, alpha, beta, 0.0f, sample_rate);
  }

  for (int v = 0; v < kNumVoices; ++v)
    voices_.emplace_back(kMinFrequency, sample_rate, 0.0f);

  retune_counter_ = 0;
  chord_step_ = 0;
  stabilize_counter_ = 0;
  last_sample_ = 0.0f;
  published_sample_ = 0.0f;
}

float ResonateModel::alphaForTimeConstant(float time_constant) const
{
  return 1.0f - std::exp(-1.0f / (static_cast<float>(sample_rate_) * time_constant));
}

void ResonateModel::processBlock(const float *in, int num_samples)
{
  if (in == nullptr)
    return;

  for (int n = 0; n < num_samples; ++n)
    pushSample(in[n]);
  publish();
}

void ResonateModel::generateSynthetic(int num_samples)
{
  for (int n = 0; n < num_samples; ++n)
  {
    if (--retune_counter_ <= 0)
    {
      retune_counter_ = static_cast<int>(sample_rate_ * 0.9);
      for (int v = 0; v < kNumVoices; ++v)
      {
        const int bin = (chord_step_ * 7 + v * 23 + 3) % kNumResonators;
        voices_[v].setFrequency(frequencies_[bin]);
        voices_[v].setAmplitude(0.8f / kNumVoices);
      }
      ++chord_step_;
    }

    float sample = 0.0f;
    for (auto &voice : voices_)
    {
      sample += voice.sample();
      voice.incrementPhase();
    }
    pushSample(sample);
  }
  publish();
}

void ResonateModel::snapshot(std::vector<float> &out_amplitudes, float &out_last_sample)
{
  std::lock_guard<std::mutex> lock(mutex_);
  out_amplitudes = amplitudes_;
  out_last_sample = published_sample_;
}

void ResonateModel::pushSample(float sample)
{
  last_sample_ = sample;
  for (auto &resonator : resonators_)
    resonator.updateWithSample(sample);

  if (++stabilize_counter_ >= kStabilizeInterval)
  {
    stabilize_counter_ = 0;
    for (auto &resonator : resonators_)
      resonator.stabilize();
  }
}

void ResonateModel::publish()
{
  std::lock_guard<std::mutex> lock(mutex_);
  for (int i = 0; i < kNumResonators; ++i)
    amplitudes_[i] = resonators_[i].amplitude();
  published_sample_ = last_sample_;
}

SpectrumView::SpectrumView(observe::ApplicationWindow &app, ResonateModel &model, bool synthesize_when_idle)
    : app_(app), model_(model), synthesize_when_idle_(synthesize_when_idle),
      amplitudes_(kNumResonators, 0.0f)
{
}

void SpectrumView::install()
{
  app_.onDraw() = [this](observe::Canvas &canvas)
  {
    const float w = app_.width();
    const float h = app_.height();

    const double now = canvas.time();
    if (last_time_ < 0.0)
      last_time_ = now;
    double dt = now - last_time_;
    last_time_ = now;
    if (dt > 0.05)
      dt = 0.05;

    if (synthesize_when_idle_)
    {
      const int samples = static_cast<int>(dt * model_.sampleRate());
      if (samples > 0)
        model_.generateSynthetic(samples);
    }

    model_.snapshot(amplitudes_, last_sample_);

    canvas.setColor(0xff101018);
    canvas.fill(0, 0, w, h);

    const float margin = h * 0.06f;
    const float spectrum_top = margin;
    const float spectrum_bottom = h - margin;
    const float spectrum_height = spectrum_bottom - spectrum_top;
    const int n = model_.size();
    const float slot = w / static_cast<float>(n);
    const float bar_w = slot * 0.7f;

    float peak = 0.0f;
    for (int i = 0; i < n; ++i)
      peak = std::max(peak, amplitudes_[i]);
    display_peak_ = std::max(peak, display_peak_ * 0.995f);
    const float scale = 1.0f / std::max(display_peak_, 1.0e-3f);

    for (int i = 0; i < n; ++i)
    {
      const float norm = std::min(1.0f, amplitudes_[i] * scale);
      const float bar_h = norm * spectrum_height;
      const float x = i * slot + (slot - bar_w) * 0.5f;
      const float y = spectrum_bottom - bar_h;

      const float hue = static_cast<float>(i) / n;
      const unsigned int r = static_cast<unsigned int>((0.25f + 0.75f * norm) * 255) & 0xff;
      const unsigned int g = static_cast<unsigned int>((0.40f + 0.55f * hue) * 255) & 0xff;
      const unsigned int b = static_cast<unsigned int>((0.55f + 0.45f * (1.0f - hue)) * 255) & 0xff;
      canvas.setColor(0xff000000u | (r << 16) | (g << 8) | b);
      canvas.roundedRectangle(x, y, bar_w, bar_h, bar_w * 0.25f);
    }

    canvas.setColor(0x40ffffff);
    const float wave_y = spectrum_top * 0.5f + last_sample_ * spectrum_top * 0.4f;
    canvas.circle(w * 0.5f - 3.0f, wave_y - 3.0f, 6.0f);

    app_.redraw();
  };
}

} // namespace oscillators_observe
