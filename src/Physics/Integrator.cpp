#include "Integrator.h"
#include <glm/gtc/constants.hpp>

struct ElectricField;
struct MagneticField;
struct Particle;
IntegratorType Integrator::type = IntegratorType::RK4;

void Integrator::integrate(Particle& p,
                          const ElectricField& eField,
                          const MagneticField& bField,
                          const float dt) {
    switch(type) {
        case IntegratorType::EULER:
            euler(p, eField, bField, dt);
            break;
        case IntegratorType::RK4:
            rk4(p, eField, bField, dt);
            break;
        case IntegratorType::VERLET:
            // Fallback to RK4 for now
            rk4(p, eField, bField, dt);
            break;
    }
    p.age += dt;
    p.addTrailPoint();
}

void Integrator::euler(Particle& p,
                       const ElectricField& eField,
                       const MagneticField& bField,
                       const float dt) {
    const float time = p.age;
    const glm::vec3 force = lorentzForce(p, eField, bField, p.position, p.velocity, time);
    p.acceleration = force / p.mass;
    p.velocity += p.acceleration * dt;
    p.position += p.velocity * dt;
}

void Integrator::rk4(Particle& p,
                     const ElectricField& eField,
                     const MagneticField& bField,
                     float dt) {
    float time = p.age;
    glm::vec3 x = p.position;
    glm::vec3 v = p.velocity;

    // k1
    glm::vec3 a1 = lorentzForce(p, eField, bField, x, v, time) / p.mass;
    glm::vec3 k1v = a1;
    glm::vec3 k1x = v;

    // k2
    glm::vec3 x2 = x + 0.5f * dt * k1x;
    glm::vec3 v2 = v + 0.5f * dt * k1v;
    glm::vec3 a2 = lorentzForce(p, eField, bField, x2, v2, time + 0.5f * dt) / p.mass;
    glm::vec3 k2v = a2;
    glm::vec3 k2x = v2;

    // k3
    glm::vec3 x3 = x + 0.5f * dt * k2x;
    glm::vec3 v3 = v + 0.5f * dt * k2v;
    glm::vec3 a3 = lorentzForce(p, eField, bField, x3, v3, time + 0.5f * dt) / p.mass;
    glm::vec3 k3v = a3;
    glm::vec3 k3x = v3;

    // k4
    glm::vec3 x4 = x + dt * k3x;
    glm::vec3 v4 = v + dt * k3v;
    glm::vec3 a4 = lorentzForce(p, eField, bField, x4, v4, time + dt) / p.mass;
    glm::vec3 k4v = a4;
    glm::vec3 k4x = v4;

    // Update
    p.velocity = v + (dt / 6.0f) * (k1v + 2.0f * k2v + 2.0f * k3v + k4v);
    p.position = x + (dt / 6.0f) * (k1x + 2.0f * k2x + 2.0f * k3x + k4x);
    p.acceleration = a1;
}

glm::vec3 Integrator::lorentzForce(const Particle& p,
                                   const ElectricField& eField,
                                   const MagneticField& bField,
                                   const glm::vec3& pos,
                                   const glm::vec3& vel,
                                   const float time) {
    const glm::vec3 E = eField.getField(pos, time);
    const glm::vec3 B = bField.getField(pos, time);
    return p.charge * (E + glm::cross(vel, B));
}