#pragma once
#include <GLFW/glfw3.h>

#include "../Physics/ParticleSystem.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Camera.h"
#include "ControlPanel.h"
#include "GraphPanel.h"

class MainWindow {
public:
    MainWindow();
    ~MainWindow();

    bool initialize(int width, int height, const char* title);
    void run();
    void shutdown();

private:
    GLFWwindow* window;
    ParticleSystem system;
    Renderer renderer;
    Camera camera;
    ControlPanel controlPanel;
    GraphPanel graphPanel;

    bool running;
    bool runningSimulation;
    float deltaTime;
    float lastFrameTime;
    float simulationTime;
    float timeAccumulator;

    void processInput();
    void updateUI();
    void renderUI();
    void setupImGui();
    void setupCallbacks();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};