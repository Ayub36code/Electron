#pragma once
#include "../Physics/ParticleSystem.h"
#include "../Graphics/Renderer.h"

class ControlPanel {
public:
    ControlPanel();
    ~ControlPanel();

    void render(ParticleSystem& system, Renderer& renderer);
    bool isVisible() const { return visible; }
    void setVisible(const bool vis) { visible = vis; }

private:
    bool visible;
    bool showFieldPresets;
    bool showParticleControls;
    bool showAdvancedSettings;

    // Field presets
    int selectedFieldPreset;
    std::vector<std::string> fieldPresetNames;

    static void renderFieldControls(ParticleSystem& system);
    static void renderParticleControls(ParticleSystem& system);

    static void renderSimulationControls(ParticleSystem& system);

    static void renderRenderControls(Renderer& renderer);
    void renderFieldPresets(ParticleSystem& system);
};