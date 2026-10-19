
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <random>

class QuaternionUtils {
public:
    /**
     * Create a quaternion from Euler angles (avoiding gimbal lock)
     * This is the "imaginary numbers" part - quaternions have 3 imaginary components (i, j, k)
     */
    static glm::quat fromEuler(float x, float y, float z) {
        return glm::quat(glm::vec3(x, y, z));
    }

    /**
     * Slerp interpolation between two quaternions (smooth rotation)
     * Uses imaginary number algebra internally
     */
    static glm::quat slerp(const glm::quat& q1, const glm::quat& q2, float t) {
        return glm::slerp(q1, q2, t);
    }

    /**
     * Create a random unit quaternion (uniform distribution on S³)
     * Uses the "imaginary sphere" method with random numbers
     */
    static glm::quat random() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

        float u1 = dis(gen);
        float u2 = dis(gen);
        float u3 = dis(gen);

        return glm::quat(
            sqrt(1.0f - u1) * sin(2.0f * 3.14159f * u2),
            sqrt(1.0f - u1) * cos(2.0f * 3.14159f * u2),
            sqrt(u1) * sin(2.0f * 3.14159f * u3),
            sqrt(u1) * cos(2.0f * 3.14159f * u3)
        );
    }

    /**
     * Rotate a vector using a quaternion (pure imaginary vector)
     * v' = q * v * q⁻¹
     */
    static glm::vec3 rotateVector(const glm::quat& quat, const glm::vec3& vector) {
        return quat * vector;
    }

    // Append inside the class:
    static glm::quat slerpPath(const std::vector<glm::quat>& keyframes, float t);
    static glm::mat4 toMatrix(const glm::quat& q);
    static glm::quat compose(const std::vector<glm::quat>& rotations);

    /**
     * Get the axis and angle from a quaternion
     * Extracts the imaginary part (i, j, k) as the axis
     */
    static void getAxisAngle(const glm::quat& quat, glm::vec3& axis, float& angle) {
        float len = sqrt(quat.x * quat.x + quat.y * quat.y + quat.z * quat.z);
        if (len > 0.001f) {
            axis = glm::vec3(quat.x / len, quat.y / len, quat.z / len);
            angle = 2.0f * acos(quat.w);
        } else {
            axis = glm::vec3(1.0f, 0.0f, 0.0f);
            angle = 0.0f;
        }
    }
};

