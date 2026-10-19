#include "../include/config.h"
#include <algorithm>
#include <cmath>

Camera::Camera()
    : target(0.0f, 0.0f, 0.0f),       // start looking at the Sun
      distance(60.0f),
      targetDistance(60.0f),
      yaw(45.0f),                     // nice 3/4 view
      pitch(25.0f),
      targetYaw(45.0f),
      targetPitch(25.0f),
      minDistance(2.0f),
      maxDistance(400.0f),
      zoomSpeed(3.0f),
      zoomSmooth(10.0f),
      minPitch(-85.0f),
      maxPitch(85.0f),
      fov(45.0f),
      nearPlane(0.1f),
      farPlane(2000.0f),
      aspectRatio(16.0f / 9.0f),
      rotateSensitivity(0.35f),
      panSensitivity(0.02f),
      leftDragging(false),
      rightDragging(false),
      lastMouseX(0.0), lastMouseY(0.0),
      deltaMouseX(0.0), deltaMouseY(0.0),
      scrollDelta(0.0),
      position(0.0f),
      front(0.0f, 0.0f, -1.0f),
      up(0.0f, 1.0f, 0.0f),
      right(1.0f, 0.0f, 0.0f) {
    updateVectors();
}

// ============================================================================
// Main update
// ============================================================================
void Camera::update(float deltaTime) {
    // --- Handle scroll zoom ---
    if (scrollDelta != 0.0) {
        // Exponential zoom (feels natural at all distances)
        float factor = std::pow(0.9f, (float)scrollDelta);
        targetDistance *= factor;
        targetDistance = glm::clamp(targetDistance, minDistance, maxDistance);
    }

    // --- Smooth distance ---
    distance = glm::mix(distance, targetDistance, glm::clamp(zoomSmooth * deltaTime, 0.0f, 1.0f));

    // --- Smooth yaw / pitch from drag ---
    float t = glm::clamp(zoomSmooth * deltaTime, 0.0f, 1.0f);
    yaw   = glm::mix(yaw, targetYaw, t);
    pitch = glm::mix(pitch, targetPitch, t);

    updateVectors();

    // Reset per-frame deltas
    deltaMouseX = 0.0;
    deltaMouseY = 0.0;
    scrollDelta = 0.0;
}

// ============================================================================
// Keyboard
// ============================================================================
void Camera::processInput(GLFWwindow* window, float deltaTime) {
    // Optional: arrow keys pan the target
    float panSpeed = distance * 0.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) target -= right * panSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) target += right * panSpeed;
    if (glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) target += up * panSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) target -= up * panSpeed;
}

// ============================================================================
// Mouse input
// ============================================================================
void Camera::onMouseMove(double xpos, double ypos) {
    if (lastMouseX == 0.0 && lastMouseY == 0.0) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        return;
    }

    float dx = (float)(xpos - lastMouseX);
    float dy = (float)(lastMouseY - ypos); // invert Y for natural feel

    lastMouseX = xpos;
    lastMouseY = ypos;

    // Left-drag: orbit
    if (leftDragging) {
        targetYaw   += dx * rotateSensitivity;
        targetPitch += dy * rotateSensitivity;
        targetPitch = glm::clamp(targetPitch, minPitch, maxPitch);
    }

    // Right-drag: pan
    if (rightDragging) {
        float panAmount = distance * panSensitivity;
        target -= right * dx * panAmount;
        target += up    * dy * panAmount;
    }
}

void Camera::onMouseButton(int button, int action, int mods) {
    bool pressed = (action == GLFW_PRESS);

    if (button == GLFW_MOUSE_BUTTON_LEFT)  leftDragging  = pressed;
    if (button == GLFW_MOUSE_BUTTON_RIGHT) rightDragging = pressed;
}

void Camera::onScroll(double xoffset, double yoffset) {
    scrollDelta = yoffset;  // handled in update()
}

void Camera::onKey(int key, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_R) {
        reset();
    }
}

// ============================================================================
// Zoom helper (call from code, e.g. animation)
// ============================================================================
void Camera::zoom(float amount) {
    targetDistance *= std::pow(0.9f, amount);
    targetDistance = glm::clamp(targetDistance, minDistance, maxDistance);
}

// ============================================================================
// Reset / focus
// ============================================================================
void Camera::reset() {
    target = glm::vec3(0.0f);
    targetDistance = 60.0f;
    distance = 60.0f;
    targetYaw = 45.0f;
    targetPitch = 25.0f;
    yaw = 45.0f;
    pitch = 25.0f;
    updateVectors();
}

void Camera::focusOn(const glm::vec3& newTarget, float newDistance) {
    target = newTarget;
    targetDistance = glm::clamp(newDistance, minDistance, maxDistance);
}

// ============================================================================
// Spherical → Cartesian
// ============================================================================
void Camera::updateVectors() {
    // Clamp pitch
    pitch = glm::clamp(pitch, minPitch, maxPitch);

    // Spherical coordinates around target
    float yawRad   = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);

    position.x = target.x + distance * cos(pitchRad) * cos(yawRad);
    position.y = target.y + distance * sin(pitchRad);
    position.z = target.z + distance * cos(pitchRad) * sin(yawRad);

    // Look at the target
    front = glm::normalize(target - position);
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up    = glm::normalize(glm::cross(right, front));
}

// ============================================================================
// Matrices
// ============================================================================
glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::mat4 Camera::getViewProjectionMatrix() const {
    return getProjectionMatrix() * getViewMatrix();
}