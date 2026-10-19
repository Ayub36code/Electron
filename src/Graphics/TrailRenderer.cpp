#include "TrailRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <../../extern/glad/include/glad/glad.h>

TrailRenderer::TrailRenderer(const size_t maxPoints)
    : color(1.0f, 0.5f, 0.0f),
      width(0.02f),
      maxPoints(maxPoints),
      fade(true),
      glow(true),
      VAO(0),
      VBO(0),
      initialized(false) {}

TrailRenderer::~TrailRenderer() {
    if(initialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void TrailRenderer::addPoint(const glm::vec3& point) {
    TrailPoint tp;
    tp.position = point;
    tp.alpha = 1.0f;
    tp.age = 0.0f;
    points.push_back(tp);

    if(points.size() > maxPoints) {
        points.pop_front();
    }

    updateVBO();
}

void TrailRenderer::clear() {
    points.clear();
}

void TrailRenderer::render(const glm::mat4& view, const glm::mat4& projection) {
    if(points.size() < 2) return;
    if(!initialized) initGL();

    // Update alpha for fading
    if(fade) {
        float totalAge = static_cast<float>(points.size()) * 0.001f;
        for(size_t i = 0; i < points.size(); ++i) {
            float t = static_cast<float>(i) / points.size();
            points[i].alpha = t * t * (3.0f - 2.0f * t); // Smoothstep
        }
        updateVBO();
    }

    // Render trail
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, points.size());
    glBindVertexArray(0);
}

void TrailRenderer::initGL() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    initialized = true;
}

void TrailRenderer::updateVBO() const {
    if(!initialized) return;
    if(points.empty()) return;

    std::vector<glm::vec3> positions;
    positions.reserve(points.size());
    for(const auto& p : points) {
        positions.push_back(p.position);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3),
                 positions.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}
