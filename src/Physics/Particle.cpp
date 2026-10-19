#include "Particle.h"
#include <glm/gtc/constants.hpp>

Particle::Particle() {
    reset();
}

void Particle::reset() {
    position = glm::vec3(0.0f);
    velocity = glm::vec3(2.0e6f, 0.0f, 0.0f);
    acceleration = glm::vec3(0.0f);
    mass = 9.109e-31f;
    charge = -1.602e-19f;
    radius = 0.1f;
    color = glm::vec3(1.0f, 0.5f, 0.0f);
    active = true;
    lifetime = 1e-6f;
    age = 0.0f;
    clearTrail();
}

void Particle::addTrailPoint() {
    trail.push_back(position);
    if (trail.size() > MAX_TRAIL) {
        trail.erase(trail.begin());
    }
}

void Particle::clearTrail() {
    trail.clear();
    trail.reserve(MAX_TRAIL);
}

Particle ParticlePreset::electron() {
    Particle p;
    p.mass = 9.109e-31f;
    p.charge = -1.602e-19f;
    p.color = glm::vec3(0.0f, 1.0f, 0.8f);
    p.radius = 0.08f;
    return p;
}

Particle ParticlePreset::positron() {
    Particle p = electron();
    p.charge = 1.602e-19f;
    p.color = glm::vec3(1.0f, 0.2f, 0.8f);
    return p;
}

Particle ParticlePreset::proton() {
    Particle p;
    p.mass = 1.673e-27f;
    p.charge = 1.602e-19f;
    p.color = glm::vec3(1.0f, 0.0f, 0.0f);
    p.radius = 0.15f;
    return p;
}

Particle ParticlePreset::custom(float mass, float charge) {
    Particle p = electron();
    p.mass = mass;
    p.charge = charge;
    p.color = glm::vec3(0.0f, 1.0f, 0.0f);
    return p;
}