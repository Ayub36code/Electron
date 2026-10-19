#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    
    void load(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    void unuse() const;
    
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setMat4(const std::string& name, const float* value) const;
    
    unsigned int getID() const { return id; }
    bool isValid() const { return valid; }
    
private:
    unsigned int id{};
    bool valid{};
    mutable std::unordered_map<std::string, int> uniformCache;
    
    int getUniformLocation(const std::string& name) const;
    bool checkCompileErrors(unsigned int shader, const std::string& type) const;
};