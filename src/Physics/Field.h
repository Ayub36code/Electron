#pragma once
#include <glm/glm.hpp>
#include <functional>
#include <string>

enum class FieldType {
    CONSTANT,
    DIPOLE,
    QUADRUPOLE,
    HELMHOLTZ,
    CUSTOM
};

struct ElectricField {
    glm::vec3 constant = glm::vec3(0.0f);
    FieldType type = FieldType::CONSTANT;
    std::function<glm::vec3(const glm::vec3&, float)> customFunc;
    
    glm::vec3 getField(const glm::vec3& pos, float time) const;
    std::string getName() const;
};

struct MagneticField {
    glm::vec3 constant = glm::vec3(0.0f, 0.0f, 1.0f);
    FieldType type = FieldType::CONSTANT;
    std::function<glm::vec3(const glm::vec3&, float)> customFunc;
    
    glm::vec3 getField(const glm::vec3& pos, float time) const;
    std::string getName() const;
};

// Field presets
struct FieldPresets {
    static ElectricField uniformE(const glm::vec3& value);
    static MagneticField uniformB(const glm::vec3& value);
    static MagneticField dipoleB(const glm::vec3& moment);
    static MagneticField helmholtzB(float coilRadius, float current);
};