#include "projectversion.h"
#include "vst3_plugin.h"

#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/vsttypes.h>
#include <public.sdk/source/main/pluginfactory.h>

#define stringPluginName "Oscillators Observe Spectrum"

BEGIN_FACTORY_DEF("Oscillators", "https://github.com/aerianthomas/observe", "")

DEF_CLASS2(INLINE_UID_FROM_FUID(kOscillatorsVst3ProcessorUID),
           Steinberg::PClassInfo::kManyInstances,
           kVstAudioEffectClass,
           stringPluginName,
           Steinberg::Vst::kDistributable,
           "Fx|Analyzer|Stereo",
           FULL_VERSION_STR,
           kVstVersionString,
           OscillatorsVst3Plugin::createInstance)

END_FACTORY
