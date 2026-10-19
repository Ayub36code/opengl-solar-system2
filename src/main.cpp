#include "../include/config.h"



// ============================================================================
// Globals
// ============================================================================
int screenWidth = 1600;
int screenHeight = 900;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool paused = false;
float speedMultiplier = 1.0f;

Camera* g_camera = nullptr;

// ============================================================================
// GLFW callbacks
// ============================================================================
void mouseCallback(GLFWwindow* w, double xpos, double ypos) {
    if (g_camera) g_camera->onMouseMove(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* w, int button, int action, int mods) {
    if (g_camera) g_camera->onMouseButton(button, action, mods);
}

void scrollCallback(GLFWwindow* w, double xoffset, double yoffset) {
    if (g_camera) g_camera->onScroll(xoffset, yoffset);
}

void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods) {
    if (g_camera) g_camera->onKey(key, action, mods);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        paused = !paused;
        std::cout << (paused ? "⏸ Paused" : "▶ Playing") << std::endl;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(w, true);
    }
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
        speedMultiplier = glm::min(speedMultiplier + 0.25f, 5.0f);
        std::cout << "Speed: " << speedMultiplier << "x" << std::endl;
    }
    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
        speedMultiplier = glm::max(speedMultiplier - 0.25f, 0.0f);
        std::cout << "Speed: " << speedMultiplier << "x" << std::endl;
    }
}

void framebufferSizeCallback(GLFWwindow* w, int width, int height) {
    screenWidth = width;
    screenHeight = height;
    glViewport(0, 0, width, height);
    if (g_camera) {
        g_camera->aspectRatio = (float)width / (float)height;
    }
}

// ============================================================================
// Main
// ============================================================================
int main() {
    // ---- GLFW ----
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight,
        "3D Solar System — Scroll to Zoom, Drag to Orbit",
        nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Normal cursor — visible so user can drag
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // ---- GLAD ----
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);

    // ---- Camera ----
    Camera camera;
    camera.aspectRatio = (float)screenWidth / (float)screenHeight;
    camera.target       = glm::vec3(0.0f);   // Sun at origin
    camera.distance     = 60.0f;
    camera.targetDistance = 60.0f;
    camera.yaw          = 45.0f;
    camera.pitch        = 25.0f;
    camera.targetYaw    = 45.0f;
    camera.targetPitch  = 25.0f;
    camera.update(0.0f);
    g_camera = &camera;

    std::cout << "\n=== Controls ===\n"
              << "  Scroll      — zoom in / out\n"
              << "  Left drag   — orbit around center\n"
              << "  Right drag  — pan the view\n"
              << "  Arrow keys  — pan target\n"
              << "  Left click planet — focus\n"
              << "  R           — reset view\n"
              << "  Space       — pause\n"
              << "  +/-         — speed\n\n";

    // ---- Shaders ----
    Shader planetShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader starShader("shaders/star_vertex.glsl", "shaders/star_fragment.glsl");
    Shader glowShader("shaders/glow_vertex.glsl", "shaders/glow_fragment.glsl");

    // ---- Systems ----
    StarField starField(15000);
    PlanetSystem planetSystem;
    AsteroidBelt asteroidBelt(3000);
    ParticleSystem particleSystem;

    // ---- Planet picking ----
    static double clickX = 0.0, clickY = 0.0;
    static bool clickPending = false;
    glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            glfwGetCursorPos(w, &clickX, &clickY);
            clickPending = true;
        }
        if (g_camera) g_camera->onMouseButton(button, action, mods);
    });

    auto pickPlanet = [&](double mx, double my) -> Planet* {
        glm::mat4 vp = camera.getViewProjectionMatrix();
        float bestDist = 40.0f;
        Planet* best = nullptr;
        for (auto& p : planetSystem.getAllPlanets()) {
            glm::vec4 clip = vp * glm::vec4(p->position, 1.0f);
            if (clip.w <= 0.0f) continue;
            glm::vec3 ndc = glm::vec3(clip) / clip.w;
            float sx = (ndc.x * 0.5f + 0.5f) * screenWidth;
            float sy = (1.0f - (ndc.y * 0.5f + 0.5f)) * screenHeight;
            float d = glm::length(glm::vec2(sx - (float)mx, sy - (float)my));
            if (d < bestDist) { bestDist = d; best = p.get(); }
        }
        return best;
    };

    // ---- Loop ----
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera.processInput(window, deltaTime);
        camera.update(deltaTime);

        if (clickPending) {
            clickPending = false;
            Planet* p = pickPlanet(clickX, clickY);
            if (p) {
                std::cout << "🎯 Focusing on " << p->data.name << std::endl;
                camera.focusOn(p->position, p->data.radius * 6.0f + 2.0f);
            }
        }

        glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float elapsed = currentFrame;
        if (!paused) {
            starField.update(elapsed);
            planetSystem.update(deltaTime * speedMultiplier, elapsed);
            asteroidBelt.update(elapsed);
            particleSystem.update(elapsed);
        }

        glm::mat4 projection = camera.getProjectionMatrix();
        glm::mat4 view = camera.getViewMatrix();

        // Stars
        starShader.use();
        starShader.setMat4("projection", projection);
        starShader.setMat4("view", view);
        starField.render(starShader);

        // Planets
        planetShader.use();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);
        planetShader.setVec3("viewPos", camera.getPosition());
        planetShader.setVec3("lightPos", glm::vec3(0.0f));
        planetSystem.render(planetShader);

        // Orbit lines
        planetShader.use();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);
        planetSystem.renderOrbitLinesPublic(planetShader);

        // Asteroids
        planetShader.use();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);
        asteroidBelt.render(planetShader);

        // Comet trail
        planetShader.use();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);
        particleSystem.render(planetShader);

        // Sun glow
        glowShader.use();
        glowShader.setMat4("projection", projection);
        glowShader.setMat4("view", view);
        glowShader.setFloat("time", elapsed);
        planetSystem.renderGlow(glowShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_camera = nullptr;
    glfwTerminate();
    return 0;
}