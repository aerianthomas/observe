#include "clap_entry_impl.h"

#include <clap/clap.h>

extern "C"
{
  const CLAP_EXPORT clap_plugin_entry clap_entry = {
      CLAP_VERSION,
      oscillators_entry_init,
      oscillators_entry_deinit,
      oscillators_entry_get_factory,
  };
}
