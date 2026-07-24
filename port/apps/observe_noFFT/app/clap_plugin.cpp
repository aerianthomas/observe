#include "clap_plugin.h"

#include <clap/helpers/host-proxy.hh>
#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <algorithm>
#include <cstdio>
#include <cstring>

using namespace observe::dimension;

namespace
{

constexpr clap_id kMainPortId = 0;

static const char *kClapFeatures[] = {
    CLAP_PLUGIN_FEATURE_AUDIO_EFFECT,
    CLAP_PLUGIN_FEATURE_ANALYZER,
    CLAP_PLUGIN_FEATURE_STEREO,
    nullptr,
};

float sampleFromInput(const clap_audio_buffer_t &input, uint32_t channel, uint32_t frame)
{
  const bool is_constant = (input.constant_mask & (1ull << channel)) != 0;
  const uint32_t sample_index = is_constant ? 0 : frame;

  if (input.data32 != nullptr)
    return input.data32[channel][sample_index];
  if (input.data64 != nullptr)
    return static_cast<float>(input.data64[channel][sample_index]);
  return 0.0f;
}

void copyOrClearOutput(const clap_process *process)
{
  if (process->audio_outputs_count == 0)
    return;

  auto &output = process->audio_outputs[0];
  const clap_audio_buffer_t *input = process->audio_inputs_count > 0 ? &process->audio_inputs[0] : nullptr;

  for (uint32_t ch = 0; ch < output.channel_count; ++ch)
  {
    const uint32_t input_ch = input != nullptr && input->channel_count > 0 ? std::min(ch, input->channel_count - 1) : 0;
    for (uint32_t frame = 0; frame < process->frames_count; ++frame)
    {
      const float sample = input != nullptr ? sampleFromInput(*input, input_ch, frame) : 0.0f;
      if (output.data32 != nullptr)
        output.data32[ch][frame] = sample;
      else if (output.data64 != nullptr)
        output.data64[ch][frame] = sample;
    }
  }
}

} // namespace

clap_plugin_descriptor OscillatorsClapPlugin::descriptor = {
    CLAP_VERSION,
    "dev.oscillators.observe.spectrum",
    "Oscillators Observe Spectrum",
    "Oscillators",
    "https://github.com/AudioKit/Oscillators",
    "",
    "",
    "0.0.1",
    "Resonator-bank spectrum analyzer",
    kClapFeatures,
};

OscillatorsClapPlugin::OscillatorsClapPlugin(const clap_host *host) : ClapPluginBase(&descriptor, host) {}

OscillatorsClapPlugin::~OscillatorsClapPlugin() = default;

bool OscillatorsClapPlugin::activate(double sample_rate,
                                     uint32_t /*min_frame_count*/,
                                     uint32_t max_frame_count) noexcept
{
  model_.setSampleRate(sample_rate);
  mono_buffer_.reserve(max_frame_count);
  return true;
}

clap_process_status OscillatorsClapPlugin::process(const clap_process *process) noexcept
{
  if (process == nullptr)
    return CLAP_PROCESS_ERROR;

  copyOrClearOutput(process);

  if (process->audio_inputs_count == 0 || process->audio_inputs[0].channel_count == 0)
    return CLAP_PROCESS_CONTINUE;

  const auto &input = process->audio_inputs[0];
  mono_buffer_.resize(process->frames_count);

  const uint32_t channels = input.channel_count;
  for (uint32_t frame = 0; frame < process->frames_count; ++frame)
  {
    float sample = 0.0f;
    for (uint32_t ch = 0; ch < channels; ++ch)
      sample += sampleFromInput(input, ch, frame);
    mono_buffer_[frame] = sample / static_cast<float>(channels);
  }

  model_.processBlock(mono_buffer_.data(), static_cast<int>(mono_buffer_.size()));
  return CLAP_PROCESS_CONTINUE;
}

uint32_t OscillatorsClapPlugin::audioPortsCount(bool /*is_input*/) const noexcept
{
  return 1;
}

bool OscillatorsClapPlugin::audioPortsInfo(uint32_t index,
                                           bool is_input,
                                           clap_audio_port_info *info) const noexcept
{
  if (index != 0 || info == nullptr)
    return false;

  info->id = kMainPortId;
  std::snprintf(info->name, sizeof(info->name), "%s", is_input ? "Input" : "Output");
  info->flags = CLAP_AUDIO_PORT_IS_MAIN | CLAP_AUDIO_PORT_SUPPORTS_64BITS;
  info->channel_count = 2;
  info->port_type = CLAP_PORT_STEREO;
  info->in_place_pair = kMainPortId;
  return true;
}

#ifdef __linux__
void OscillatorsClapPlugin::onPosixFd(int fd, clap_posix_fd_flags_t flags) noexcept
{
  if (app_ && app_->window())
    app_->window()->processPluginFdEvents();
}
#endif

bool OscillatorsClapPlugin::guiIsApiSupported(const char *api, bool is_floating) noexcept
{
  if (is_floating)
    return false;

#ifdef _WIN32
  if (std::strcmp(api, CLAP_WINDOW_API_WIN32) == 0)
    return true;
#elif __APPLE__
  if (std::strcmp(api, CLAP_WINDOW_API_COCOA) == 0)
    return true;
#elif __linux__
  if (std::strcmp(api, CLAP_WINDOW_API_X11) == 0)
    return true;
#endif

  return false;
}

bool OscillatorsClapPlugin::guiCreate(const char * /*api*/, bool is_floating) noexcept
{
  if (is_floating)
    return false;

  if (app_)
    return true;

  app_ = std::make_unique<observe::ApplicationWindow>();
  app_->setWindowDimensions(100_vmin, 60_vmin);

  spectrum_ = std::make_unique<oscillators_observe::SpectrumView>(*app_, model_, false);
  spectrum_->install();

  app_->onWindowContentsResized() = [this] { _host.guiRequestResize(pluginWidth(), pluginHeight()); };
  return true;
}

void OscillatorsClapPlugin::guiDestroy() noexcept
{
#if __linux__
  if (app_ && app_->window() && _host.canUsePosixFdSupport())
    _host.posixFdSupportUnregister(app_->window()->posixFd());
#endif

  if (app_)
    app_->close();
  spectrum_ = nullptr;
  app_ = nullptr;
}

bool OscillatorsClapPlugin::guiSetParent(const clap_window *window) noexcept
{
  if (app_ == nullptr)
    return false;

  app_->show(window->ptr);

#if __linux__
  if (_host.canUsePosixFdSupport() && app_->window())
  {
    int fd_flags = CLAP_POSIX_FD_READ | CLAP_POSIX_FD_WRITE | CLAP_POSIX_FD_ERROR;
    return _host.posixFdSupportRegister(app_->window()->posixFd(), fd_flags);
  }
#endif
  return true;
}

bool OscillatorsClapPlugin::guiGetResizeHints(clap_gui_resize_hints_t *hints) noexcept
{
  if (app_ == nullptr)
    return false;

  const bool fixed_aspect_ratio = app_->isFixedAspectRatio();
  hints->can_resize_horizontally = true;
  hints->can_resize_vertically = true;
  hints->preserve_aspect_ratio = fixed_aspect_ratio;

  if (fixed_aspect_ratio)
  {
    hints->aspect_ratio_width = app_->height() * app_->aspectRatio();
    hints->aspect_ratio_height = app_->width();
  }
  return true;
}

bool OscillatorsClapPlugin::guiAdjustSize(uint32_t *width, uint32_t *height) noexcept
{
  if (app_ == nullptr)
    return false;

  app_->adjustWindowDimensions(width, height, true, true);
  return true;
}

bool OscillatorsClapPlugin::guiSetSize(uint32_t width, uint32_t height) noexcept
{
  if (app_ == nullptr)
    return false;

  setPluginDimensions(width, height);
  return true;
}

bool OscillatorsClapPlugin::guiGetSize(uint32_t *width, uint32_t *height) noexcept
{
  if (app_ == nullptr)
    return false;

  *width = pluginWidth();
  *height = pluginHeight();
  return true;
}

int OscillatorsClapPlugin::pluginWidth() const
{
  if (app_ == nullptr)
    return 0;

#if __APPLE__
  return app_->width();
#else
  return app_->nativeWidth();
#endif
}

int OscillatorsClapPlugin::pluginHeight() const
{
  if (app_ == nullptr)
    return 0;

#if __APPLE__
  return app_->height();
#else
  return app_->nativeHeight();
#endif
}

void OscillatorsClapPlugin::setPluginDimensions(int width, int height)
{
  if (app_ == nullptr)
    return;

#if __APPLE__
  app_->setWindowDimensions(width, height);
#else
  app_->setNativeWindowDimensions(width, height);
#endif
}
