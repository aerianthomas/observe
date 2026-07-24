#pragma once

bool oscillators_entry_init(const char *plugin_path);
void oscillators_entry_deinit();
const void *oscillators_entry_get_factory(const char *factory_id);
