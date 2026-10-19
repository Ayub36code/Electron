#include "MainWindow.h"
#include <../../extern/glad/include/glad/glad.h>
#include <GLFW/glfw3.h>
#include "../../extern/imgui/imgui.h"
#include "../../extern/backends/imgui_impl_glfw.h"
#include "../../extern//backends/imgui_impl_opengl3.h"
#include <iostream>

MainWindow::MainWindow()
    : window(nullptr),
      running(true),
      runningSimulation(false),
      deltaTime(0.0f),
      lastFrameTime(0.0f),
      simulationTime(0.0f),
      timeAccumulator(0.0f) {}

MainWindow::~MainWindow() {
    shutdown();
}

bool MainWindow::initialize(const int width, const int height, const char* title) {
    if(!glfwInit()) {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!window) {
        std::cout << "Failed to create window!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD!" << std::endl;
        return false;
    }

    renderer.initialize(width, height);

    // Add default particle
    Particle electron = ParticlePreset::electron();
    electron.position = glm::vec3(0.0f);
    electron.velocity = glm::vec3(2.0e6f, 0.0f, 0.0f);
    system.addParticle(electron);

    setupCallbacks();
    setupImGui();

    std::cout << "Electron Simulation initialized successfully!" << std::endl;
    return true;
}

void MainWindow::setupCallbacks() {
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetWindowUserPointer(window, this);
}

void MainWindow::setupImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

void MainWindow::run() {
    while(!glfwWindowShouldClose(window) && running) {
        const float currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        if(deltaTime > 0.1f) deltaTime = 0.1f;

        processInput();

        // Update simulation
        if(runningSimulation) {
            timeAccumulator += deltaTime;
            float dt = system.getDt();
            while(timeAccumulator >= dt) {
                system.update(dt);
                simulationTime += dt;
                timeAccumulator -= dt;
            }

            // Update graphs
            graphPanel.update(system);
        }

        // Render
        renderer.render(system, camera);

        // UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderUI();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void MainWindow::processInput() {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Space to toggle simulation
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        runningSimulation = !runningSimulation;
    }

    // R to reset
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        system.reset();
        runningSimulation = false;
    }
}

void MainWindow::renderUI() {
    // Main menu bar
    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Reset Simulation", "Ctrl+R")) {
                system.reset();
                runningSimulation = false;
            }
            if(ImGui::MenuItem("Add Particle")) {
                system.addParticle(ParticlePreset::electron());
            }
            if(ImGui::MenuItem("Export Trajectory")) {
                // Export functionality
            }
            if(ImGui::MenuItem("Exit", "Ctrl+Q")) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Control Panel", NULL, &controlPanel.visible);
            ImGui::MenuItem("Graph Panel", NULL, &graphPanel.visible);
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Camera")) {
            static int camMode = 0;
            if(ImGui::Combo("Mode", &camMode, "Orbit\0First Person\0Follow\0")) {
                camera.setMode(static_cast<CameraMode>(camMode));
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("Controls", NULL, false, false);
            ImGui::MenuItem("About", NULL, false, false);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Status bar
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 20));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 20));
    ImGui::Begin("Status Bar", NULL,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    ImGui::Text("Time: %.3e s | Particles: %zu | FPS: %.1f",
                simulationTime,
                system.getParticleCount(),
                ImGui::GetIO().Framerate);

    if(runningSimulation) {
        ImGui::SameLine(ImGui::GetIO().DisplaySize.x - 100);
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "▶ RUNNING");
    } else {
        ImGui::SameLine(ImGui::GetIO().DisplaySize.x - 100);
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "⏸ PAUSED");
    }

    ImGui::End();

    // Render panels
    controlPanel.render(system, renderer);
    graphPanel.render();

    // Particle info overlay
    const auto& particles = system.getParticles();
    if(!particles.empty()) {
        const auto& p = particles[0];
        ImGui::Begin("Particle Info", NULL,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("Position: (%.2e, %.2e, %.2e)", p.position.x, p.position.y, p.position.z);
        ImGui::Text("Velocity: (%.2e, %.2e, %.2e)", p.velocity.x, p.velocity.y, p.velocity.z);
        ImGui::Text("Speed: %.2e m/s", glm::length(p.velocity));
        ImGui::Text("Energy: %.2e J", system.getKineticEnergy(p));
        ImGui::End();
    }
}

void MainWindow::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if(window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

// Callbacks
void MainWindow::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    MainWindow* app = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    if(app) {
        app->renderer.resize(width, height);
    }
}

void MainWindow::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    MainWindow* app = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    if(app && !ImGui::GetIO().WantCaptureMouse) {
        static double lastX = xpos, lastY = ypos;
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            app->camera.processMouse((float)(xpos - lastX), (float)(ypos - lastY));
        }
        lastX = xpos;
        lastY = ypos;
    }
}

void MainWindow::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    MainWindow* app = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    if(app && !ImGui::GetIO().WantCaptureMouse) {
        app->camera.processScroll(static_cast<float>(yoffset));
    }
}

void MainWindow::keyCallback(GLFWwindow* window, const int key, int scancode, const int action, int mods) {
    MainWindow* app = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    if(app && action == GLFW_PRESS) {
        if(key == GLFW_KEY_SPACE) {
            app->runningSimulation = !app->runningSimulation;
        }
        if(key == GLFW_KEY_R) {
            app->system.reset();
            app->runningSimulation = false;
        }
    }
}