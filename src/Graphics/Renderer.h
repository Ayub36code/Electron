#pragma once
#include "Camera.h"
#include "Shader.h"
#include "../Physics/ParticleSystem.h"
#include <vector>
#include <glm/glm.hpp>

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool initialize(int width, int height);
    void render(const ParticleSystem& system, const Camera& camera);
    void renderTrails(const ParticleSystem& system, const Camera& camera) const;
    void renderGrid(const Camera& camera) const;
    void renderAxes(const Camera& camera) const;
    
    void setClearColor(const glm::vec3& color) { clearColor = color; }
    void setShowTrails(bool show) { showTrails = show; }
    void setShowGrid(bool show) { showGrid = show; }
    void setShowAxes(bool show) { showAxes = show; }
    void setTrailLength(int length) { trailLength = length; }
    
    void resize(int width, int height);
    
private:
    Shader particleShader;
    Shader trailShader;
    Shader gridShader;
    Shader axisShader;
    
    unsigned int VAO, VBO, EBO;
    unsigned int trailVAO, trailVBO;
    unsigned int gridVAO, gridVBO;
    unsigned int axisVAO, axisVBO;
    
    glm::vec3 clearColor;
    bool showTrails;
    bool showGrid;
    bool showAxes;
    int trailLength;
    
    int screenWidth;
    int screenHeight;
    
    void setupParticleVAO();
    void setupTrailVAO();
    void setupGridVAO();
    void setupAxisVAO();
    
    void renderParticle(const Particle& p, const Camera& camera);
};