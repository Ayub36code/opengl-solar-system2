#pragma once
#include "../include/config.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    // ---- Target & orbit state ----
    glm::vec3 target;         // point the camera looks at (world origin = Sun)
    float distance;           // current distance from target
    float targetDistance;     // desired distance (for smooth zoom)
    float yaw;                // horizontal orbit angle (degrees)
    float pitch;              // vertical orbit angle (degrees)
    float targetYaw;
    float targetPitch;

    // ---- Zoom limits ----
    float minDistance;
    float maxDistance;
    float zoomSpeed;          // per scroll tick
    float zoomSmooth;         // lerp factor

    // ---- Orbit limits ----
    float minPitch;
    float maxPitch;

    // ---- Projection ----
    float fov;
    float nearPlane;
    float farPlane;
    float aspectRatio;

    // ---- Input ----
    float rotateSensitivity;
    float panSensitivity;
    bool leftDragging;
    bool rightDragging;
    double lastMouseX, lastMouseY;
    double deltaMouseX, deltaMouseY;
    double scrollDelta;

    // ---- Derived ----
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    // ---- Constructor ----
    Camera();

    // ---- Main update ----
    void update(float deltaTime);
    void processInput(GLFWwindow* window, float deltaTime);

    // ---- Input handlers ----
    void onMouseMove(double xpos, double ypos);
    void onMouseButton(int button, int action, int mods);
    void onScroll(double xoffset, double yoffset);
    void onKey(int key, int action, int mods);

    // ---- View / projection ----
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewProjectionMatrix() const;

    // ---- Helpers ----
    void reset();
    void focusOn(const glm::vec3& newTarget, float newDistance = 20.0f);
    void zoom(float amount);

    // ---- Getters ----
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getTarget() const { return target; }
    float getFov() const { return fov; }

private:
    void updateVectors();
};