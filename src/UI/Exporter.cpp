#include "Exporter.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>

Exporter::Exporter()
    : precision(6),
      includeHeader(true) {}

Exporter::~Exporter() = default;

bool Exporter::exportTrajectory(const ParticleSystem& system,
                                const std::string& filename,
                                ExportFormat format) {
    Logger::getInstance().info("Exporting trajectory to: " + filename);

    switch(format) {
        case ExportFormat::CSV:
            return exportCSV(system, filename);
        case ExportFormat::JSON:
            return exportJSON(system, filename);
        case ExportFormat::VTK:
            return exportVTK(system, filename);
        default:
            Logger::getInstance().error("Unknown export format");
            return false;
    }
}

bool Exporter::exportCSV(const ParticleSystem& system, const std::string& filename) const {
    std::ofstream file(filename);
    if(!file.is_open()) {
        Logger::getInstance().error("Failed to open file: " + filename);
        return false;
    }

    const auto& particles = system.getParticles();
    if(particles.empty()) {
        Logger::getInstance().warning("No particles to export");
        return true;
    }

    const auto& p = particles[0];
    const auto& trail = p.trail;

    // Header
    if(includeHeader) {
        file << "Time,X,Y,Z,Vx,Vy,Vz\n";
    }

    // Data
    const float dt = system.getDt();
    for(size_t i = 0; i < trail.size(); ++i) {
        const auto& pos = trail[i];
        // For velocity, we'd need to compute derivatives
        file << std::scientific << std::setprecision(precision)
             << (i * dt) << ","
             << pos.x << "," << pos.y << "," << pos.z << ","
             << "0,0,0\n";
    }

    file.close();
    Logger::getInstance().info("Exported " + std::to_string(trail.size()) + " points");
    return true;
}

bool Exporter::exportJSON(const ParticleSystem& system, const std::string& filename) const {
    std::ofstream file(filename);
    if(!file.is_open()) {
        Logger::getInstance().error("Failed to open file: " + filename);
        return false;
    }

    const auto& particles = system.getParticles();
    if(particles.empty()) {
        Logger::getInstance().warning("No particles to export");
        return true;
    }

    const auto& p = particles[0];
    const auto& trail = p.trail;

    file << "{\n";
    file << "  \"metadata\": {\n";
    file << "    \"timestamp\": \"" << getTimestamp() << "\",\n";
    file << "    \"particle_type\": \"electron\",\n";
    file << "    \"num_points\": " << trail.size() << "\n";
    file << "  },\n";
    file << "  \"trajectory\": [\n";

    for(size_t i = 0; i < trail.size(); ++i) {
        const auto& pos = trail[i];
        file << "    [" << pos.x << ", " << pos.y << ", " << pos.z << "]";
        if(i < trail.size() - 1) file << ",";
        file << "\n";
    }

    file << "  ]\n";
    file << "}\n";

    file.close();
    return true;
}

bool Exporter::exportVTK(const ParticleSystem& system, const std::string& filename) {
    std::ofstream file(filename);
    if(!file.is_open()) {
        Logger::getInstance().error("Failed to open file: " + filename);
        return false;
    }

    const auto& particles = system.getParticles();
    if(particles.empty()) {
        Logger::getInstance().warning("No particles to export");
        return true;
    }

    const auto& p = particles[0];
    const auto& trail = p.trail;

    file << "# vtk DataFile Version 3.0\n";
    file << "Electron trajectory\n";
    file << "ASCII\n";
    file << "DATASET POLYDATA\n";
    file << "POINTS " << trail.size() << " float\n";

    for(const auto& pos : trail) {
        file << pos.x << " " << pos.y << " " << pos.z << "\n";
    }

    file << "LINES 1 " << trail.size() + 1 << "\n";
    file << trail.size();
    for(size_t i = 0; i < trail.size(); ++i) {
        file << " " << i;
    }
    file << "\n";

    file.close();
    return true;
}

bool Exporter::exportScreenshot(const std::string& filename) {
    Logger::getInstance().info("Exporting screenshot to: " + filename);
    // Implementation would use OpenGL glReadPixels
    return true;
}

bool Exporter::exportAnimation(const std::string& filename) {
    Logger::getInstance().info("Exporting animation to: " + filename);
    // Implementation would capture frames and encode video
    return true;
}

std::string Exporter::formatNumber(float value) const {
    std::stringstream ss;
    ss << std::scientific << std::setprecision(precision) << value;
    return ss.str();
}

std::string Exporter::getTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}