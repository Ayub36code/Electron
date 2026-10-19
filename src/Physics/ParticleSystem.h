#pragma once
#include "Particle.h"
#include "Field.h"
#include "Integrator.h"
#include <vector>
#include <mutex>
#include <thread>

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();
    
    // Particle management
    void addParticle(const Particle& p);
    void removeParticle(size_t index);
    void clearParticles();
    size_t getParticleCount() const { return particles.size(); }
    
    // Simulation
    void update(float dt);
    void reset();
    
    // Settings
    void setIntegrator(IntegratorType type) { Integrator::type = type; }
    void setElectricField(const ElectricField& field) { eField = field; }
    void setMagneticField(const MagneticField& field) { bField = field; }
    void setDt(float dt) { this->dt = dt; }
    
    // Access
    const std::vector<Particle>& getParticles() const { return particles; }
    ElectricField& getElectricField() { return eField; }
    MagneticField& getMagneticField() { return bField; }
    float getDt() const { return dt; }
    float getTime() const { return time; }
    
    // Statistics
    float getTotalEnergy() const;

    static float getKineticEnergy(const Particle& p);
    
private:
    std::vector<Particle> particles;
    ElectricField eField;
    MagneticField bField;
    IntegratorType integratorType = IntegratorType::RK4;
    float dt = 1e-12f;
    float time = 0.0f;
    bool useMultiThreading = false;
    std::mutex particleMutex;
    
    void updateParticle(Particle& p, float dt) const;
};