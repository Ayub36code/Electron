#pragma once
#include "Particle.h"
#include "Field.h"

enum class IntegratorType {
    EULER,
    RK4,
    VERLET
};

class Integrator {
public:
    static IntegratorType type;
    
    static void integrate(Particle& p, 
                         const ElectricField& eField,
                         const MagneticField& bField,
                         float dt);
    
private:
    static void euler(Particle& p, 
                      const ElectricField& eField,
                      const MagneticField& bField,
                      float dt);
    
    static void rk4(Particle& p,
                    const ElectricField& eField,
                    const MagneticField& bField,
                    float dt);
    
    static glm::vec3 lorentzForce(const Particle& p,
                                  const ElectricField& eField,
                                  const MagneticField& bField,
                                  const glm::vec3& pos,
                                  const glm::vec3& vel,
                                  float time);
};