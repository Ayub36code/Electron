#include "ControlPanel.h"
#include "../../extern/imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>

ControlPanel::ControlPanel() 
    : visible(true), 
      showFieldPresets(true),
      showParticleControls(true),
      showAdvancedSettings(false),
      selectedFieldPreset(0) {
    fieldPresetNames = {
        "Uniform B",
        "Dipole B",
        "Helmholtz B",
        "Crossed E&B",
        "Custom"
    };
}

ControlPanel::~ControlPanel() = default;

void ControlPanel::render(ParticleSystem& system, Renderer& renderer) {
    if(!visible) return;
    
    ImGui::Begin("Control Panel", &visible);
    
    renderFieldControls(system);
    renderParticleControls(system);
    renderSimulationControls(system);
    renderRenderControls(renderer);
    renderFieldPresets(system);
    
    ImGui::End();
}

void ControlPanel::renderFieldControls(ParticleSystem& system) {
    ImGui::Separator();
    ImGui::Text("⚡ Field Controls");
    
    auto& eField = system.getElectricField();
    auto& bField = system.getMagneticField();
    
    // E Field
    if(ImGui::CollapsingHeader("Electric Field", ImGuiTreeNodeFlags_DefaultOpen)) {
        constexpr float eStep = 1e4f;
        constexpr float eMin = -1e6f;
        constexpr float eMax = 1e6f;
        ImGui::DragFloat3("E (N/C)", glm::value_ptr(eField.constant), eStep, eMin, eMax, "%.2e");
        
        // E field presets
        if(ImGui::Button("Set E=0")) {
            eField.constant = glm::vec3(0.0f);
        }
        ImGui::SameLine();
        if(ImGui::Button("Set E=x")) {
            eField.constant = glm::vec3(1e5f, 0.0f, 0.0f);
        }
        ImGui::SameLine();
        if(ImGui::Button("Set E=y")) {
            eField.constant = glm::vec3(0.0f, 1e5f, 0.0f);
        }
        ImGui::SameLine();
        if(ImGui::Button("Set E=z")) {
            eField.constant = glm::vec3(0.0f, 0.0f, 1e5f);
        }
    }
    
    // B Field
    if(ImGui::CollapsingHeader("Magnetic Field", ImGuiTreeNodeFlags_DefaultOpen)) {
        const float bStep = 0.1f;
        const float bMin = -10.0f;
        const float bMax = 10.0f;
        ImGui::DragFloat3("B (T)", glm::value_ptr(bField.constant), bStep, bMin, bMax, "%.2f");
        
        // B field presets
        if(ImGui::Button("Set B=0")) {
            bField.constant = glm::vec3(0.0f);
        }
        ImGui::SameLine();
        if(ImGui::Button("Set B=x")) {
            bField.constant = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        ImGui::SameLine();
        if(ImGui::Button("Set B=y")) {
            bField.constant = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        ImGui::SameLine();
        if(ImGui::Button("Set B=z")) {
            bField.constant = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        
        // B field strength slider
        static float bStrength = 1.0f;
        if(ImGui::SliderFloat("B Strength", &bStrength, 0.0f, 10.0f)) {
            bField.constant = glm::normalize(bField.constant) * bStrength;
        }
    }
}

void ControlPanel::renderParticleControls(ParticleSystem& system) {
    ImGui::Separator();
    ImGui::Text("🔬 Particle Controls");
    
    auto& particles = system.getParticles();
    if(particles.empty()) return;
    
    auto& p = particles[0];
    
    ImGui::Text("Particle Type: %s", "Electron");
    ImGui::Text("Mass: %.2e kg", p.mass);
    ImGui::Text("Charge: %.2e C", p.charge);
    
    // Initial conditions
    if(ImGui::CollapsingHeader("Initial Conditions", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Position (m)", glm::value_ptr(p.position), 1e-6f, -1e-3f, 1e-3f, "%.2e");
        ImGui::DragFloat3("Velocity (m/s)", glm::value_ptr(p.velocity), 1e5f, -1e7f, 1e7f, "%.2e");
    }
    
    // Particle presets
    if(ImGui::CollapsingHeader("Particle Presets")) {
        if(ImGui::Button("Electron")) {
            p = ParticlePreset::electron();
        }
        ImGui::SameLine();
        if(ImGui::Button("Positron")) {
            p = ParticlePreset::positron();
        }
        ImGui::SameLine();
        if(ImGui::Button("Proton")) {
            p = ParticlePreset::proton();
        }
    }
}

void ControlPanel::renderSimulationControls(ParticleSystem& system) {
    ImGui::Separator();
    ImGui::Text("🎮 Simulation Controls");
    
    float dt = system.getDt();
    if(ImGui::DragFloat("Time Step (ps)", &dt, 0.1e-12f, 1e-14f, 1e-10f, "%.2e")) {
        system.setDt(dt);
    }
    
    static bool running = false;
    if(ImGui::Button(running ? "⏸ Stop" : "▶ Start")) {
        running = !running;
    }
    ImGui::SameLine();
    if(ImGui::Button("⏹ Reset")) {
        system.reset();
        running = false;
    }
    ImGui::SameLine();
    if(ImGui::Button("⏭ Step")) {
        system.update(system.getDt());
    }
    
    // Simulation speed
    static float speed = 1.0f;
    if(ImGui::SliderFloat("Speed", &speed, 0.1f, 100.0f)) {
        // Adjust simulation speed
    }
    
    // Integrator selection
    static int integrator = 1;
    if(ImGui::Combo("Integrator", &integrator, "Euler\0RK4\0Verlet\0")) {
        system.setIntegrator(static_cast<IntegratorType>(integrator));
    }
}

void ControlPanel::renderRenderControls(Renderer& renderer) {
    ImGui::Separator();
    ImGui::Text("🎨 Render Settings");
    
    ImGui::Checkbox("Show Trails", &renderer.showTrails);
    ImGui::Checkbox("Show Grid", &renderer.showGrid);
    ImGui::Checkbox("Show Axes", &renderer.showAxes);
    
    int trailLen = renderer.trailLength;
    if(ImGui::SliderInt("Trail Length", &trailLen, 100, 10000)) {
        renderer.trailLength = trailLen;
    }
    
    // Color picker for background
    static float clearColor[3] = {0.1f, 0.1f, 0.15f};
    if(ImGui::ColorEdit3("Background", clearColor)) {
        renderer.setClearColor(glm::vec3(clearColor[0], clearColor[1], clearColor[2]));
    }
    
    // Particle size
    static float particleSize = 1.0f;
    if(ImGui::SliderFloat("Particle Size", &particleSize, 0.1f, 5.0f)) {
        // Update particle size
    }
}

void ControlPanel::renderFieldPresets(ParticleSystem& system) {
    ImGui::Separator();
    ImGui::Text("📋 Field Presets");
    
    if(ImGui::Combo("Preset", &selectedFieldPreset, 
                    "Uniform B\0Dipole B\0Helmholtz B\0Crossed E&B\0Custom\0")) {
        // Apply preset
        switch(selectedFieldPreset) {
            case 0: // Uniform B
                system.setMagneticField(FieldPresets::uniformB(glm::vec3(0.0f, 0.0f, 1.0f)));
                system.setElectricField(FieldPresets::uniformE(glm::vec3(0.0f)));
                break;
            case 1: // Dipole B
                system.setMagneticField(FieldPresets::uniformB(glm::vec3(0.0f, 0.0f, 1.0f)));
                system.setElectricField(FieldPresets::uniformE(glm::vec3(0.0f)));
                break;
            case 2: // Helmholtz B
                system.setMagneticField(FieldPresets::uniformB(glm::vec3(0.0f, 0.0f, 1.0f)));
                system.setElectricField(FieldPresets::uniformE(glm::vec3(0.0f)));
                break;
            case 3: // Crossed E&B
                system.setMagneticField(FieldPresets::uniformB(glm::vec3(0.0f, 0.0f, 1.0f)));
                system.setElectricField(FieldPresets::uniformE(glm::vec3(1e5f, 0.0f, 0.0f)));
                break;
            case 4: // Custom
                // User-defined fields
                break;
        }
    }
}