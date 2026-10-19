#include "ParticleSystem.h"
#include <algorithm>
#include <numeric>

ParticleSystem::ParticleSystem() {
    // Default fields
    eField = FieldPresets::uniformE(glm::vec3(0.0f));
    bField = FieldPresets::uniformB(glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Add default electron
    Particle electron = ParticlePreset::electron();
    electron.position = glm::vec3(0.0f);
    electron.velocity = glm::vec3(2.0e6f, 0.0f, 0.0f);
    particles.push_back(electron);
}

ParticleSystem::~ParticleSystem() {
    clearParticles();
}

void ParticleSystem::addParticle(const Particle& p) {
    std::lock_guard<std::mutex> lock(particleMutex);
    particles.push_back(p);
}

void ParticleSystem::removeParticle(const size_t index) {
    std::lock_guard<std::mutex> lock(particleMutex);
    if(index < particles.size()) {
        particles.erase(particles.begin() + index);
    }
}

void ParticleSystem::clearParticles() {
    std::lock_guard<std::mutex> lock(particleMutex);
    particles.clear();
}

void ParticleSystem::update(float dt) {
    std::lock_guard<std::mutex> lock(particleMutex);
    
    if(useMultiThreading) {
        // Multi-threaded update
        std::vector<std::thread> threads;
        size_t numThreads = std::thread::hardware_concurrency();
        size_t particlesPerThread = particles.size() / numThreads;
        
        for(size_t t = 0; t < numThreads; ++t) {
            size_t start = t * particlesPerThread;
            size_t end = (t == numThreads - 1) ? particles.size() : start + particlesPerThread;
            threads.emplace_back([this, start, end, dt]() {
                for(size_t i = start; i < end; ++i) {
                    updateParticle(particles[i], dt);
                }
            });
        }
        
        for(auto& thread : threads) {
            thread.join();
        }
    } else {
        // Single-threaded update
        for(auto& p : particles) {
            updateParticle(p, dt);
        }
    }
    
    time += dt;
}

void ParticleSystem::updateParticle(Particle& p, float dt) const {
    if(!p.active) return;
    Integrator::integrate(p, eField, bField, dt);
}

void ParticleSystem::reset() {
    std::lock_guard<std::mutex> lock(particleMutex);
    for(auto& p : particles) {
        p.reset();
    }
    time = 0.0f;
}

float ParticleSystem::getTotalEnergy() const {
    float total = 0.0f;
    for(const auto& p : particles) {
        total += getKineticEnergy(p);
    }
    return total;
}

float ParticleSystem::getKineticEnergy(const Particle& p) {
    return 0.5f * p.mass * glm::dot(p.velocity, p.velocity);
}