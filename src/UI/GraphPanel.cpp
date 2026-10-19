#include "GraphPanel.h"

#include <algorithm>
#include "../../extern/imgui/imgui.h"
#include <glm/gtc/constants.hpp>
#include <utility>

GraphData::GraphData(const size_t size, std::string  label, const glm::vec3& color)
    : minVal(0.0f), maxVal(1.0f), maxSize(size), label(std::move(label)), color(color) {
    clear();
}

void GraphData::addPoint(const float value) {
    data.push_back(value);
    if(data.size() > maxSize) {
        data.pop_front();
    }
    
    // Update min/max
    if(data.size() > 1) {
        minVal = *std::min_element(data.begin(), data.end());
        maxVal = *std::max_element(data.begin(), data.end());
        const float range = maxVal - minVal;
        if(range < 1e-10f) {
            minVal -= 0.5f;
            maxVal += 0.5f;
        } else {
            minVal -= range * 0.1f;
            maxVal += range * 0.1f;
        }
    }
}

void GraphData::clear() {
    data.clear();
    minVal = 0.0f;
    maxVal = 1.0f;
}

GraphPanel::GraphPanel() 
    : visible(true),
      selectedGraph(0) {
    graphOptions = {
        "Position",
        "Velocity",
        "Energy",
        "Speed"
    };
    
    positionX = GraphData(1000, "X", glm::vec3(1.0f, 0.0f, 0.0f));
    positionY = GraphData(1000, "Y", glm::vec3(0.0f, 1.0f, 0.0f));
    positionZ = GraphData(1000, "Z", glm::vec3(0.0f, 0.0f, 1.0f));
    
    velocityX = GraphData(1000, "Vx", glm::vec3(1.0f, 0.0f, 0.0f));
    velocityY = GraphData(1000, "Vy", glm::vec3(0.0f, 1.0f, 0.0f));
    velocityZ = GraphData(1000, "Vz", glm::vec3(0.0f, 0.0f, 1.0f));
    
    kineticEnergy = GraphData(1000, "Kinetic", glm::vec3(1.0f, 0.8f, 0.0f));
    potentialEnergy = GraphData(1000, "Potential", glm::vec3(0.0f, 0.8f, 1.0f));
    totalEnergy = GraphData(1000, "Total", glm::vec3(1.0f, 0.0f, 1.0f));
    speedGraph = GraphData(1000, "Speed", glm::vec3(0.0f, 1.0f, 0.0f));
}

GraphPanel::~GraphPanel() = default;

void GraphPanel::update(const ParticleSystem& system) {
    updateGraphs(system);
}

void GraphPanel::updateGraphs(const ParticleSystem& system) {
    const auto& particles = system.getParticles();
    if(particles.empty()) return;
    
    const auto& p = particles[0];
    
    // Position
    positionX.addPoint(p.position.x);
    positionY.addPoint(p.position.y);
    positionZ.addPoint(p.position.z);
    
    // Velocity
    velocityX.addPoint(p.velocity.x);
    velocityY.addPoint(p.velocity.y);
    velocityZ.addPoint(p.velocity.z);
    
    // Energy
    float KE = system.getKineticEnergy(p);
    kineticEnergy.addPoint(KE);
    
    // Speed
    const float speed = glm::length(p.velocity);
    speedGraph.addPoint(speed);
}

void GraphPanel::render() {
    if(!visible) return;
    
    ImGui::Begin("Graph Panel", &visible);

    auto graphSize = ImVec2(400, 150);
    auto graphSizeSmall = ImVec2(400, 80);
    
    if(ImGui::BeginTabBar("GraphTabs")) {
        if(ImGui::BeginTabItem("Position")) {
            renderPositionGraphs(graphSize);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Velocity")) {
            renderVelocityGraphs(graphSize);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Energy")) {
            renderEnergyGraphs(graphSize);
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Speed")) {
            renderGraph("Speed", speedGraph, graphSize);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}

void GraphPanel::renderGraph(const char* title, GraphData& graph, const ImVec2& size) {
    if(graph.data.empty()) {
        ImGui::Text("No data yet...");
        return;
    }
    
    std::vector<float> data(graph.data.begin(), graph.data.end());
    
    char overlay[256];
    snprintf(overlay, sizeof(overlay), "%s\n[%.2e, %.2e]", 
             graph.label.c_str(), graph.minVal, graph.maxVal);
    
    ImGui::PlotLines(title, data.data(), data.size(), 0, overlay,
                     graph.minVal, graph.maxVal, size);
}

void GraphPanel::renderPositionGraphs(const ImVec2& size) {
    renderGraph("X Position", positionX, size);
    renderGraph("Y Position", positionY, size);
    renderGraph("Z Position", positionZ, size);
}

void GraphPanel::renderVelocityGraphs(const ImVec2& size) {
    renderGraph("X Velocity", velocityX, size);
    renderGraph("Y Velocity", velocityY, size);
    renderGraph("Z Velocity", velocityZ, size);
}

void GraphPanel::renderEnergyGraphs(const ImVec2& size) {
    renderGraph("Kinetic Energy", kineticEnergy, size);
    renderGraph("Total Energy", totalEnergy, size);
}