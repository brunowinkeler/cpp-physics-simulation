#include "GasPreset.h"

#include <array>
#include <cstddef>

namespace physim
{
    namespace
    {
        constexpr float AVOGADRO_NUMBER = 6.02214076e23f;

        constexpr std::array<GasPreset, 4> GAS_PRESETS{{
            {GasPresetId::Helium, "Helium", 4.002602e-3f, 4.002602e-3f / AVOGADRO_NUMBER, 0.012f, 300.0f},
            {GasPresetId::Air, "Air / Nitrogen", 28.97e-3f, 28.97e-3f / AVOGADRO_NUMBER, 0.014f, 300.0f},
            {GasPresetId::Argon, "Argon", 39.948e-3f, 39.948e-3f / AVOGADRO_NUMBER, 0.015f, 300.0f},
            {GasPresetId::CarbonDioxide, "Carbon Dioxide", 44.01e-3f, 44.01e-3f / AVOGADRO_NUMBER, 0.016f, 300.0f},
        }};
    }

    const GasPreset &getGasPreset(GasPresetId id)
    {
        return GAS_PRESETS[static_cast<std::size_t>(id)];
    }

    int getGasPresetCount()
    {
        return static_cast<int>(GAS_PRESETS.size());
    }

    GasPresetId getGasPresetId(int index)
    {
        return GAS_PRESETS[static_cast<std::size_t>(index)].id;
    }

    int getGasPresetIndex(GasPresetId id)
    {
        return static_cast<int>(id);
    }
} // namespace physim
