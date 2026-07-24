#include "clap_entry_impl.h"

#include "clap_plugin.h"

#include <cstring>

namespace
{

uint32_t getPluginCount(const clap_plugin_factory *factory)
{
  return 1;
}

const clap_plugin_descriptor *getPluginDescriptor(const clap_plugin_factory *factory, uint32_t index)
{
  return index == 0 ? &OscillatorsClapPlugin::descriptor : nullptr;
}

const clap_plugin *createPlugin(const clap_plugin_factory *factory,
                                const clap_host *host,
                                const char *plugin_id)
{
  if (std::strcmp(plugin_id, OscillatorsClapPlugin::descriptor.id) != 0)
    return nullptr;

  auto *plugin = new OscillatorsClapPlugin(host);
  return plugin->clapPlugin();
}

} // namespace

bool oscillators_entry_init(const char *plugin_path)
{
  return true;
}

void oscillators_entry_deinit() {}

const void *oscillators_entry_get_factory(const char *factory_id)
{
  static constexpr clap_plugin_factory plugin_factory = {
      getPluginCount,
      getPluginDescriptor,
      createPlugin,
  };
  return !std::strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) ? &plugin_factory : nullptr;
}
