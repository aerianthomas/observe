#include <observe/app.h>
#include <observe/ui.h>

#include "miniaudio.h"
#include "resonate_view.h"

namespace
{

void audioCallback(ma_device *device, void * /*output*/, const void *input, ma_uint32 frame_count)
{
  auto *model = static_cast<oscillators_observe::ResonateModel *>(device->pUserData);
  model->processBlock(static_cast<const float *>(input), static_cast<int>(frame_count));
}

} // namespace

int runApp()
{
  observe::ApplicationWindow app;
  oscillators_observe::ResonateModel model;

  ma_device device;
  ma_device_config config = ma_device_config_init(ma_device_type_loopback);
  config.capture.format = ma_format_f32;
  config.capture.channels = 1;
  config.sampleRate = static_cast<ma_uint32>(model.sampleRate());
  config.dataCallback = audioCallback;
  config.pUserData = &model;

  bool audio_live = ma_device_init(nullptr, &config, &device) == MA_SUCCESS;
  if (audio_live && ma_device_start(&device) != MA_SUCCESS)
  {
    ma_device_uninit(&device);
    audio_live = false;
  }

  oscillators_observe::SpectrumView spectrum(app, model, !audio_live);
  spectrum.install();

  app.setTitle(audio_live ? "Oscillators - Resonate Spectrum (Loopback)"
                          : "Oscillators - Resonate Spectrum (No Output - Synthetic)");

  observe::PopupMenu menu_bar;

  observe::PopupMenu file_menu("File");
  file_menu.addOption(1, "Exit").onSelection().add([&app](int) { app.close(); });

  observe::PopupMenu view_menu("View");
  view_menu.addOption(2, "Always on Top").select(app.isAlwaysOnTop()).onSelection().add(
      [&app](int) { app.setWindowOnTop(!app.isAlwaysOnTop()); });

  observe::PopupMenu help_menu("Help");
  help_menu.addOption(3, "About Oscillators");

  menu_bar.addSubMenu(std::move(file_menu));
  menu_bar.addSubMenu(std::move(view_menu));
  menu_bar.addSubMenu(std::move(help_menu));

  menu_bar.setAsNativeMenuBar();

  app.show(1000, 600);
  app.runEventLoop();

  if (audio_live)
    ma_device_uninit(&device);
  return 0;
}

#if defined(_WIN32)
#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  return runApp();
}
#else
int main(int, char **)
{
  return runApp();
}
#endif
