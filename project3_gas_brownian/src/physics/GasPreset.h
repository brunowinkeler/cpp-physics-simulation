#ifndef GAS_PRESET_H
#define GAS_PRESET_H

namespace physim
{
    enum class GasPresetId
    {
        Helium = 0,
        Air,
        Argon,
        CarbonDioxide,
    };

    struct GasPreset
    {
        GasPresetId id;
        const char *name;
        float molarMassKgPerMol;
        float particleMassKg;
        float effectiveParticleRadius;
        float defaultTemperatureKelvin;
    };

    const GasPreset &getGasPreset(GasPresetId id);
    int getGasPresetCount();
    GasPresetId getGasPresetId(int index);
    int getGasPresetIndex(GasPresetId id);
} // namespace physim

#endif // GAS_PRESET_H
