#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Particle {
    glm::vec3 position{};     // Position (m)
    glm::vec3 velocity{};     // Velocity (m/s)
    glm::vec3 acceleration{}; // Acceleration (m/s²)
    float mass{};             // Mass (kg)
    float charge{};           // Charge (C)
    float radius{};           // Visual radius
    glm::vec3 color{};        // Color
    bool active{};
    float lifetime{};
    float age{};
    
    // Trail
    std::vector<glm::vec3> trail;
    static constexpr size_t MAX_TRAIL = 5000;
    
    Particle();
    void reset();
    void addTrailPoint();
    void clearTrail();
};

struct ParticlePreset {
    static Particle electron();
    static Particle positron();
    static Particle proton();
    static Particle custom(float mass, float charge);
};