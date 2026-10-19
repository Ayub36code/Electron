#pragma once
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

namespace MathUtils {
    // Interpolation
    template<typename T>
    T lerp(const T& a, const T& b, float t) {
        return a + (b - a) * t;
    }
    
    // Smoothstep
    template<typename T>
    T smoothstep(const float edge0, const float edge1, const float x) {
        float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
    
    // Clamp
    template<typename T>
    T clamp(const T& value, const T& min, const T& max) {
        return std::max(min, std::min(value, max));
    }
    
    // Spherical coordinates
    inline glm::vec3 sphericalToCartesian(const float theta, const float phi, const float radius) {
        return glm::vec3(
            radius * sin(theta) * cos(phi),
            radius * cos(theta),
            radius * sin(theta) * sin(phi)
        );
    }
    
    // Cylindrical coordinates
    inline glm::vec3 cylindricalToCartesian(float rho, float phi, float z) {
        return glm::vec3(
            rho * cos(phi),
            rho * sin(phi),
            z
        );
    }
    
    // Distance squared
    inline float dist2(const glm::vec3& a, const glm::vec3& b) {
        return glm::dot(a - b, a - b);
    }
    
    // Unit vector with magnitude
    inline std::pair<glm::vec3, float> normalizeWithMagnitude(const glm::vec3& v) {
        float mag = glm::length(v);
        if(mag < 1e-10f) return {glm::vec3(0.0f), 0.0f};
        return {v / mag, mag};
    }
}