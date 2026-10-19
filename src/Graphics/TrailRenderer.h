#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <deque>

class TrailRenderer {
public:
    TrailRenderer(size_t maxPoints = 5000);
    ~TrailRenderer();
    
    void addPoint(const glm::vec3& point);
    void clear();
    void render(const glm::mat4& view, const glm::mat4& projection);
    
    void setColor(const glm::vec3& color) { this->color = color; }
    void setWidth(float width) { this->width = width; }
    void setMaxPoints(size_t max) { maxPoints = max; }
    void setFade(bool fade) { this->fade = fade; }
    void setGlow(bool glow) { this->glow = glow; }
    
private:
    struct TrailPoint {
        glm::vec3 position;
        float alpha;
        float age;
    };
    
    std::deque<TrailPoint> points;
    glm::vec3 color;
    float width;
    size_t maxPoints;
    bool fade;
    bool glow;
    
    unsigned int VAO, VBO;
    bool initialized;
    
    void initGL();
    void updateVBO() const;
};