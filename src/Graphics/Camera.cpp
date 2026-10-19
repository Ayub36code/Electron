#include "Camera.h"
#include <cmath>
#include <glm/gtc/constants.hpp>

Camera::Camera()
    : position(0.0f, 0.0f, 5.0f),
      front(0.0f, 0.0f, -1.0f),
      worldUp(0.0f, 1.0f, 0.0f),
      orbitTarget(0.0f),
      yaw(-90.0f),
      pitch(0.0f),
      zoom(45.0f),
      movementSpeed(2.5f),
      mouseSensitivity(0.1f),
      mode(CameraMode::ORBIT),
      firstMouse(true),
      lastX(0.0f),
      lastY(0.0f) {
    updateVectors();
}

void Camera::update(float deltaTime) {
    // For orbit mode, maintain distance
    if(mode == CameraMode::ORBIT) {
        const glm::vec3 offset = position - orbitTarget;
        position = orbitTarget + glm::length(offset) * front;
    }
}

void Camera::processKeyboard(const int key, const float deltaTime) {
    const float velocity = movementSpeed * deltaTime;
    if(mode == CameraMode::FIRST_PERSON) {
        switch(key) {
            case 'W': position += front * velocity; break;
            case 'S': position -= front * velocity; break;
            case 'A': position -= right * velocity; break;
            case 'D': position += right * velocity; break;
            case 'Q': position += up * velocity; break;
            case 'E': position -= up * velocity; break;
        }
    }
}

void Camera::processMouse(float xoffset, float yoffset) {
    if(firstMouse) {
        lastX = xoffset;
        lastY = yoffset;
        firstMouse = false;
        return;
    }
    
    float xdelta = xoffset - lastX;
    float ydelta = lastY - yoffset;
    lastX = xoffset;
    lastY = yoffset;
    
    xdelta *= mouseSensitivity;
    ydelta *= mouseSensitivity;
    
    yaw += xdelta;
    pitch += ydelta;
    
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
    
    updateVectors();
}

void Camera::processScroll(float yoffset) {
    zoom -= yoffset;
    if(zoom < 1.0f) zoom = 1.0f;
    if(zoom > 120.0f) zoom = 120.0f;
}

glm::mat4 Camera::getViewMatrix() const {
    if(mode == CameraMode::ORBIT) {
        return glm::lookAt(position, orbitTarget, up);
    }
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(const float aspect) const {
    return glm::perspective(glm::radians(zoom), aspect, 0.1f, 100.0f);
}

void Camera::followParticle(const glm::vec3& target) {
    if(mode == CameraMode::FOLLOW_PARTICLE) {
        orbitTarget = target;
        position = target + front * glm::length(position - orbitTarget);
    }
}

void Camera::updateVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}