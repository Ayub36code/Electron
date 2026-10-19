cmake_minimum_required(VERSION 4.3)
project(Electron)

set(CMAKE_CXX_STANDARD 14)

include_directories(src/Physics)


add_executable(Electron
        shaders/fragment.glsl
        shaders/particle.glsl
        shaders/postprocess.glsl
        shaders/vertex.glsl
        src/Physics/Particle.cpp
        src/Physics/Particle.h
        src/main.cpp
        src/Physics/ParticleSystem.cpp
        src/Physics/ParticleSystem.h
        src/Physics/Field.cpp
        src/Physics/Field.h
        src/Physics/Integrator.h
        src/Graphics/Renderer.cpp
        src/Graphics/Renderer.h
        src/Graphics/Camera.cpp
        src/Graphics/Camera.h
        src/Graphics/Shader.cpp
        src/Graphics/Shader.h
        src/Graphics/TrailRenderer.h
        src/UI/MainWindow.cpp
        src/UI/MainWindow.h
        src/UI/ControlPanel.cpp
        src/UI/ControlPanel.h
        src/UI/GraphPanel.h
        src/Utils/Logger.h
        src/Utils/Exporter.h
        src/Utils/MathUtils.h
        src/Physics/Integrator.cpp
        src/UI/GraphPanel.cpp
        src/UI/Logger.cpp
        src/UI/Logger.h
        src/UI/Exporter.cpp
        src/UI/Exporter.h
        src/UI/MathUtils.cpp
        src/UI/MathUtils.h
        src/Graphics/TrailRenderer.cpp
        src/Graphics/TrailRenderer.h)
