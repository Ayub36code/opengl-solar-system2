
#include "../include/config.h"
// All methods are static and defined inline in the header,
// but this file exists for any future non-inline implementations.

// Example: batch slerp for camera paths
glm::quat QuaternionUtils::slerpPath(const std::vector<glm::quat>& keyframes, float t) {
    if (keyframes.empty()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    if (keyframes.size() == 1) return keyframes[0];

    float scaled = t * (keyframes.size() - 1);
    int idx = static_cast<int>(glm::floor(scaled));
    if (idx >= static_cast<int>(keyframes.size()) - 1)
        return keyframes.back();

    float localT = scaled - static_cast<float>(idx);
    return glm::slerp(keyframes[idx], keyframes[idx + 1], localT);
}

// Convert quaternion to a rotation matrix (used in model matrices)
glm::mat4 QuaternionUtils::toMatrix(const glm::quat& q) {
    return glm::toMat4(q);
}

// Compose multiple rotations without gimbal lock
glm::quat QuaternionUtils::compose(const std::vector<glm::quat>& rotations) {
    glm::quat result(1.0f, 0.0f, 0.0f, 0.0f);
    for (const auto& q : rotations) {
        result = q * result;
    }
    return glm::normalize(result);
}