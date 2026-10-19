#pragma once
#include "../Physics/ParticleSystem.h"
#include <string>
#include <fstream>

enum class ExportFormat {
    CSV,
    JSON,
    VTK
};

class Exporter {
public:
    Exporter();
    ~Exporter();
    
    bool exportTrajectory(const ParticleSystem& system, 
                          const std::string& filename,
                          ExportFormat format = ExportFormat::CSV);

    static bool exportScreenshot(const std::string& filename);

    static bool exportAnimation(const std::string& filename);
    
    void setPrecision(int precision) { this->precision = precision; }
    void setIncludeHeader(bool include) { includeHeader = include; }
    
private:
    int precision;
    bool includeHeader;
    
    bool exportCSV(const ParticleSystem& system, const std::string& filename) const;
    bool exportJSON(const ParticleSystem& system, const std::string& filename) const;
    static bool exportVTK(const ParticleSystem& system, const std::string& filename);
    
    std::string formatNumber(float value) const;

    static std::string getTimestamp();
};