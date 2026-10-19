#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMode {
    ORBIT,
    FIRST_PERSON,
    FOLLOW_PARTICLE
};

class Camera {
public:
    Camera();

    void update(float deltaTime);
    void processKeyboard(int key, float deltaTime);
    void processMouse(float xoffset, float yoffset);
    void processScroll(float yoffset);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }

    void setMode(CameraMode mode) { this->mode = mode; }
    CameraMode getMode() const { return mode; }
    void followParticle(const glm::vec3& target);
    void setOrbitTarget(const glm::vec3& target) { orbitTarget = target; }

    void setPosition(const glm::vec3& pos) { position = pos; }
    void setZoom(float zoom) { this->zoom = zoom; }
    float getZoom() const { return zoom; }

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up{};
    glm::vec3 right{};
    glm::vec3 worldUp;
    glm::vec3 orbitTarget;

    float yaw;
    float pitch;
    float zoom;

    float movementSpeed;
    float mouseSensitivity;

    CameraMode mode;
    bool firstMouse;
    float lastX;
    float lastY;

    void updateVectors();
};