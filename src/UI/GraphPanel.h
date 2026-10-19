#pragma once
#include "../Physics/ParticleSystem.h"
#include <vector>
#include <deque>

struct ImVec2;

struct GraphData {
    std::deque<float> data;
    float minVal;
    float maxVal;
    size_t maxSize;
    std::string label;
    glm::vec3 color;
    
    GraphData(size_t size = 1000, std::string  label = "",
              const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));
    void addPoint(float value);
    void clear();
};

class GraphPanel {
public:
    GraphPanel();
    ~GraphPanel();
    
    void update(const ParticleSystem& system);
    void render();
    bool isVisible() const { return visible; }
    void setVisible(bool vis) { visible = vis; }
    
private:
    bool visible;
    int selectedGraph;
    std::vector<std::string> graphOptions;
    
    // Graphs
    GraphData positionX, positionY, positionZ;
    GraphData velocityX, velocityY, velocityZ;
    GraphData kineticEnergy, potentialEnergy, totalEnergy;
    GraphData speedGraph;
    
    void updateGraphs(const ParticleSystem& system);

    static void renderGraph(const char* title, GraphData& graph, const ImVec2& size);
    void renderEnergyGraphs(const ImVec2& size);
    void renderPositionGraphs(const ImVec2& size);
    void renderVelocityGraphs(const ImVec2& size);

};