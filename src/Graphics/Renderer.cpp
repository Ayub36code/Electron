#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <../../extern/glad/include/glad/glad.h>
#include <iostream>

Renderer::Renderer()
    : clearColor(0.1f, 0.1f, 0.15f),
      showTrails(true),
      showGrid(true),
      showAxes(true),
      trailLength(5000),
      screenWidth(1280),
      screenHeight(720),
      VAO(0), VBO(0), EBO(0),
      trailVAO(0), trailVBO(0),
      gridVAO(0), gridVBO(0),
      axisVAO(0), axisVBO(0) {}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &trailVAO);
    glDeleteBuffers(1, &trailVBO);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &axisVAO);
    glDeleteBuffers(1, &axisVBO);
}

bool Renderer::initialize(const int width, const int height) {
    screenWidth = width;
    screenHeight = height;
    
    particleShader.load("shaders/particle.glsl", "shaders/fragment.glsl");
    trailShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    if(!particleShader.isValid() || !trailShader.isValid()) {
        std::cout << "Failed to load shaders!" << std::endl;
        return false;
    }
    
    setupParticleVAO();
    setupTrailVAO();
    setupGridVAO();
    setupAxisVAO();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    return true;
}

void Renderer::render(const ParticleSystem& system, const Camera& camera) {
    glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render grid
    if(showGrid) renderGrid(camera);
    
    // Render axes
    if(showAxes) renderAxes(camera);
    
    // Render particles
    const auto& particles = system.getParticles();
    particleShader.use();
    particleShader.setMat4("view", camera.getViewMatrix());
    particleShader.setMat4("projection", camera.getProjectionMatrix(static_cast<float>(screenWidth)/screenHeight));
    
    for(const auto& p : particles) {
        renderParticle(p, camera);
    }
    
    // Render trails
    if(showTrails) renderTrails(system, camera);
}

void Renderer::renderParticle(const Particle& p, const Camera& camera) {
    if(!p.active) return;
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, p.position);
    model = glm::scale(model, glm::vec3(p.radius));
    
    particleShader.setMat4("model", model);
    particleShader.setVec3("aColor", p.color);
    particleShader.setFloat("aSize", p.radius * 50.0f);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer::renderTrails(const ParticleSystem& system, const Camera& camera) const {
    const auto& particles = system.getParticles();
    trailShader.use();
    trailShader.setMat4("view", camera.getViewMatrix());
    trailShader.setMat4("projection", camera.getProjectionMatrix(static_cast<float>(screenWidth)/screenHeight));
    
    for(const auto& p : particles) {
        const auto& trail = p.trail;
        if(trail.size() < 2) continue;

        const size_t start = (trail.size() > static_cast<size_t>(trailLength)) ? trail.size() - trailLength : 0;
        
        std::vector<glm::vec3> points;
        for(size_t i = start; i < trail.size(); ++i) {
            points.push_back(trail[i]);
        }
        
        glBindVertexArray(trailVAO);
        glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        
        trailShader.setMat4("model", glm::mat4(1.0f));
        trailShader.setVec3("color", p.color * 0.8f);
        glDrawArrays(GL_LINE_STRIP, 0, points.size());
    }
}

void Renderer::renderGrid(const Camera& camera) const {
    gridShader.use();
    gridShader.setMat4("view", camera.getViewMatrix());
    gridShader.setMat4("projection", camera.getProjectionMatrix((float)screenWidth/screenHeight));
    gridShader.setMat4("model", glm::mat4(1.0f));
    gridShader.setVec3("color", glm::vec3(0.3f, 0.3f, 0.4f));
    
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, 40);
}

void Renderer::renderAxes(const Camera& camera) const {
    axisShader.use();
    axisShader.setMat4("view", camera.getViewMatrix());
    axisShader.setMat4("projection", camera.getProjectionMatrix((float)screenWidth/screenHeight));
    axisShader.setMat4("model", glm::mat4(1.0f));
    
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6);
}

void Renderer::setupParticleVAO() {
    // Sphere geometry (simplified - icosahedron)
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f
    };
    
    unsigned int indices[] = {
        0,1,2, 0,2,3, 4,5,6, 4,6,7,
        8,9,10, 8,10,11, 12,13,14, 12,14,15,
        16,17,18, 16,18,19, 20,21,22, 20,22,23
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::setupTrailVAO() {
    glGenVertexArrays(1, &trailVAO);
    glGenBuffers(1, &trailVBO);
    
    glBindVertexArray(trailVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trailVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::setupGridVAO() {
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    
    float gridSize = 2.0f;
    float step = 0.2f;
    std::vector<glm::vec3> vertices;
    
    for(float x = -gridSize; x <= gridSize; x += step) {
        vertices.push_back(glm::vec3(x, 0.0f, -gridSize));
        vertices.push_back(glm::vec3(x, 0.0f, gridSize));
    }
    for(float z = -gridSize; z <= gridSize; z += step) {
        vertices.push_back(glm::vec3(-gridSize, 0.0f, z));
        vertices.push_back(glm::vec3(gridSize, 0.0f, z));
    }
    
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::setupAxisVAO() {
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);
    
    // X (red), Y (green), Z (blue)
    float vertices[] = {
        0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f
    };
    
    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::resize(const int width, const int height) {
    screenWidth = width;
    screenHeight = height;
    glViewport(0, 0, width, height);
}