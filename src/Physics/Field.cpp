#include "Field.h"
#include <cmath>
#include <glm/gtc/constants.hpp>

glm::vec3 ElectricField::getField(const glm::vec3& pos, const float time) const {
    switch(type) {
        case FieldType::CONSTANT:
            return constant;
        case FieldType::CUSTOM:
            if(customFunc) return customFunc(pos, time);
            return glm::vec3(0.0f);
        default:
            return constant;
    }
}

std::string ElectricField::getName() const {
    switch(type) {
        case FieldType::CONSTANT: return "Constant E";
        case FieldType::CUSTOM: return "Custom E";
        default: return "Unknown E";
    }
}

glm::vec3 MagneticField::getField(const glm::vec3& pos, const float time) const {
    switch(type) {
        case FieldType::CONSTANT:
            return constant;
        case FieldType::DIPOLE: {
            // Simple dipole: B = μ₀/4π * (3(m·r̂)r̂ - m)/r³
            const float r = glm::length(pos);
            if(r < 1e-10f) return glm::vec3(0.0f);
            const glm::vec3 m = glm::vec3(0.0f, 0.0f, 1.0f) * 1.0f; // Magnetic moment
            glm::vec3 rhat = glm::normalize(pos);
            const float r3 = r * r * r;
            return (3.0f * glm::dot(m, rhat) * rhat - m) / r3;
        }
        case FieldType::HELMHOLTZ: {
            // Simplified Helmholtz coil field
            const float R = 1.0f; // Coil radius
            const float z = pos.z;
            const float rho2 = pos.x * pos.x + pos.y * pos.y;
            const float rho = sqrt(rho2);
            if(rho < 1e-10f) {
                // On axis
                const float Bz = 0.8f / pow(R * R + z * z, 1.5f);
                return glm::vec3(0.0f, 0.0f, Bz);
            }
            // Off-axis (simplified)
            const float Bz = 0.8f * R * R / pow(R * R + z * z, 1.5f);
            return glm::vec3(0.0f, 0.0f, Bz);
        }
        case FieldType::CUSTOM:
            if(customFunc) return customFunc(pos, time);
            return constant;
        default:
            return constant;
    }
}

std::string MagneticField::getName() const {
    switch(type) {
        case FieldType::CONSTANT: return "Constant B";
        case FieldType::DIPOLE: return "Dipole B";
        case FieldType::HELMHOLTZ: return "Helmholtz B";
        case FieldType::CUSTOM: return "Custom B";
        default: return "Unknown B";
    }
}

ElectricField FieldPresets::uniformE(const glm::vec3& value) {
    ElectricField f;
    f.constant = value;
    f.type = FieldType::CONSTANT;
    return f;
}

MagneticField FieldPresets::uniformB(const glm::vec3& value) {
    MagneticField f;
    f.constant = value;
    f.type = FieldType::CONSTANT;
    return f;
}