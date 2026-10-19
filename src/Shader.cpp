#include "../include/config.h"

// Shader is header-only for simplicity, but here are the
// non-inline implementations for advanced use.

void Shader::setVec2(const std::string& name, const glm::vec2& vec) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setQuat(const std::string& name, const glm::quat& q) const {
    // Convert quaternion to rotation matrix and upload
    glm::mat4 mat = glm::toMat4(q);
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setFloatArray(const std::string& name, const float* values, int count) const {
    glUniform1fv(glGetUniformLocation(ID, name.c_str()), count, values);
}

void Shader::setVec3Array(const std::string& name, const glm::vec3* values, int count) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), count, glm::value_ptr(values[0]));
}

void Shader::destroy() {
    if (ID != 0) {
        glDeleteProgram(ID);
        ID = 0;
    }
}