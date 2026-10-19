
#include "../include/config.h"
#include "../include/PlanetSystem.h"


// Planet data using complex numbers for orbits (imaginary numbers!)
static const std::vector<PlanetData> PLANET_DATA = {
    {"Mercury", 0.4f, 6.0f, 4.15f, glm::vec3(0.69f, 0.66f, 0.63f), 0.03f, false},
    {"Venus", 0.7f, 9.0f, 1.62f, glm::vec3(0.91f, 0.80f, 0.63f), 2.64f, false},
    {"Earth", 0.8f, 12.0f, 1.0f, glm::vec3(0.27f, 0.53f, 1.0f), 0.41f, false},
    {"Mars", 0.5f, 15.0f, 0.53f, glm::vec3(0.76f, 0.27f, 0.05f), 0.44f, false},
    {"Jupiter", 1.8f, 20.0f, 0.084f, glm::vec3(0.83f, 0.65f, 0.45f), 0.05f, false},
    {"Saturn", 1.5f, 26.0f, 0.034f, glm::vec3(0.92f, 0.84f, 0.72f), 0.47f, true},
    {"Uranus", 1.0f, 32.0f, 0.012f, glm::vec3(0.49f, 0.78f, 0.89f), 1.71f, false},
    {"Neptune", 0.95f, 38.0f, 0.006f, glm::vec3(0.23f, 0.30f, 0.61f), 0.49f, false}
};

PlanetSystem::PlanetSystem() {
    createAll();
}

PlanetSystem::~PlanetSystem() {
    // Cleanup OpenGL resources
    glDeleteVertexArrays(1, &sunVAO);
    glDeleteBuffers(1, &sunVBO);
    glDeleteBuffers(1, &sunEBO);
    glDeleteVertexArrays(1, &glowVAO);
    glDeleteBuffers(1, &glowVBO);
    glDeleteBuffers(1, &glowEBO);
}

void PlanetSystem::createAll() {
    createSun();

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    for (const auto& data : PLANET_DATA) {
        auto planet = std::make_unique<Planet>();
        planet->data = data;

        // Random phase using imaginary numbers influence
        const float phase = dis(gen) * 2.0f * 3.14159f;

        // Create orbit with slight ellipse (imaginary numbers!)
        const float a = data.orbitRadius * 0.98f;
        const float b = data.orbitRadius * 1.02f;
        planet->orbit = ComplexOrbit(a, b, data.speed * 0.5f, phase);

        // Store tilt as quaternion (axial tilt using quaternions!)
        planet->tiltQuat = QuaternionUtils::fromEuler(data.tilt, 0.0f, 0.0f);
        planet->orientation = planet->tiltQuat;
        planet->rotationAngle = 0.0f;

        // Random orbital inclination (small random tilt)
        const float inclination = (dis(gen) - 0.5f) * 0.1f;
        planet->orientation = glm::quat(glm::vec3(0.0f, 0.0f, inclination)) * planet->orientation;

        createPlanetMesh(*planet);
        if (data.hasRings) {
            createRingMesh(*planet);
        }

        planets.push_back(std::move(planet));
    }
}

void PlanetSystem::createSun() {
    // Sun sphere
    float radius = 3.5f;
    constexpr int segments = 64;
    constexpr int rings = 64;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= rings; i++) {
        float v = static_cast<float>(i) / rings;
        const float phi = v * 3.14159f;

        for (int j = 0; j <= segments; j++) {
            float u = static_cast<float>(j) / segments;
            const float theta = u * 2.0f * 3.14159f;

            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normals
            float nx = sin(phi) * cos(theta);
            float ny = cos(phi);
            float nz = sin(phi) * sin(theta);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            // UVs
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (int i = 0; i < rings; i++) {
        for (int j = 0; j < segments; j++) {
            int a = i * (segments + 1) + j;
            int b = a + segments + 1;
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);
            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }

    sunIndexCount = indices.size();
    setupMesh(sunVAO, sunVBO, sunEBO,
              vertices.data(), vertices.size() / 8,
              indices.data(), indices.size());

    // Glow sphere (larger, transparent)
    radius = 4.5f;
    vertices.clear();
    indices.clear();

    for (int i = 0; i <= rings; i++) {
        float v = (float)i / rings;
        const float phi = v * 3.14159f;
        for (int j = 0; j <= segments; j++) {
            float u = (float)j / segments;
            float theta = u * 2.0f * 3.14159f;
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(sin(phi) * cos(theta));
            vertices.push_back(cos(phi));
            vertices.push_back(sin(phi) * sin(theta));
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (int i = 0; i < rings; i++) {
        for (int j = 0; j < segments; j++) {
            int a = i * (segments + 1) + j;
            int b = a + segments + 1;
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);
            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }

    glowIndexCount = indices.size();
    setupMesh(glowVAO, glowVBO, glowEBO,
              vertices.data(), vertices.size() / 8,
              indices.data(), indices.size());
}

void PlanetSystem::createPlanetMesh(Planet& planet) {
    float radius = planet.data.radius;
    int segments = 32;
    int rings = 32;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= rings; i++) {
        float v = (float)i / rings;
        float phi = v * 3.14159f;
        for (int j = 0; j <= segments; j++) {
            float u = (float)j / segments;
            float theta = u * 2.0f * 3.14159f;
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(sin(phi) * cos(theta));
            vertices.push_back(cos(phi));
            vertices.push_back(sin(phi) * sin(theta));
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (int i = 0; i < rings; i++) {
        for (int j = 0; j < segments; j++) {
            int a = i * (segments + 1) + j;
            int b = a + segments + 1;
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);
            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }

    planet.indexCount = indices.size();
    setupMesh(planet.VAO, planet.VBO, planet.EBO,
              vertices.data(), vertices.size() / 8,
              indices.data(), indices.size());
}

void PlanetSystem::createRingMesh(Planet& planet) {
    float innerRadius = planet.data.radius * 1.3f;
    float outerRadius = planet.data.radius * 2.2f;
    int segments = 64;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= segments; i++) {
        float u = (float)i / segments;
        float angle = u * 2.0f * 3.14159f;

        float x1 = innerRadius * cos(angle);
        float z1 = innerRadius * sin(angle);
        float x2 = outerRadius * cos(angle);
        float z2 = outerRadius * sin(angle);

        vertices.push_back(x1);
        vertices.push_back(0.0f);
        vertices.push_back(z1);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(u);
        vertices.push_back(0.0f);

        vertices.push_back(x2);
        vertices.push_back(0.0f);
        vertices.push_back(z2);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(u);
        vertices.push_back(1.0f);
    }

    for (int i = 0; i < segments; i++) {
        int a = i * 2;
        int b = a + 1;
        int c = a + 2;
        int d = a + 3;
        indices.push_back(a);
        indices.push_back(b);
        indices.push_back(c);
        indices.push_back(b);
        indices.push_back(d);
        indices.push_back(c);
    }

    planet.ringIndexCount = indices.size();
    setupMesh(planet.ringVAO, planet.ringVBO, planet.ringEBO,
              vertices.data(), vertices.size() / 8,
              indices.data(), indices.size());
}

void PlanetSystem::setupMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO,
                             float* vertices, int vertCount, unsigned int* indices, int idxCount) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertCount * 8 * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void PlanetSystem::update(float deltaTime, float elapsed) {
    float speed = speedMultiplier;

    for (auto& planet : planets) {
        // Complex orbit (imaginary numbers!) - get position
        glm::vec2 pos2d = planet->orbit.getPosition(elapsed * speed);
        planet->position = glm::vec3(pos2d.x, 0.0f, pos2d.y);

        // Daily rotation using quaternions (rotate around tilted local Y)
        planet->rotationAngle += deltaTime * 0.5f * speed;
        glm::quat rotQuat = glm::angleAxis(planet->rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        planet->orientation = planet->tiltQuat * rotQuat;

        // Add slight wobble using quaternion slerp (imaginary numbers influence)
        float wobble = sin(elapsed * 0.5f + planet->data.radius) * 0.002f;
        if (fabs(wobble) > 0.0001f) {
            glm::quat wobbleQuat = glm::angleAxis(wobble, glm::vec3(1.0f, 0.0f, 0.0f));
            planet->orientation = glm::slerp(planet->orientation, wobbleQuat * planet->orientation, 0.1f);
        }
    }
}

void PlanetSystem::render(Shader& shader) {
    // Render Sun
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", glm::vec3(1.0f, 0.87f, 0.27f));
    shader.setFloat("emissive", 1.5f);

    glBindVertexArray(sunVAO);
    glDrawElements(GL_TRIANGLES, sunIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Render planets
    for (const auto& planet : planets) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, planet->position);
        model = model * glm::toMat4(planet->orientation);
        model = glm::scale(model, glm::vec3(1.0f));

        shader.setMat4("model", model);
        shader.setVec3("objectColor", planet->data.color);
        shader.setFloat("emissive", 0.05f);

        glBindVertexArray(planet->VAO);
        glDrawElements(GL_TRIANGLES, planet->indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Render rings if present
        if (planet->data.hasRings) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, planet->position);
            // Orient rings to match planet
            model = model * glm::toMat4(planet->orientation);

            shader.setMat4("model", model);
            shader.setVec3("objectColor", glm::vec3(0.76f, 0.70f, 0.50f));
            shader.setFloat("emissive", 0.0f);

            glBindVertexArray(planet->ringVAO);
            glDrawElements(GL_TRIANGLES, planet->ringIndexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
}

void PlanetSystem::renderGlow(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);

    glBindVertexArray(glowVAO);
    glDrawElements(GL_TRIANGLES, glowIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Planet* PlanetSystem::getPlanet(const std::string& name) {
    for (auto& planet : planets) {
        if (planet->data.name == name) {
            return planet.get();
        }
    }
    return nullptr;
}

void PlanetSystem::setSpeed(float multiplier) {
    speedMultiplier = multiplier;
}
void PlanetSystem::createOrbitLine(const Planet& planet) {
    constexpr int segments = 128;
    std::vector<float> vertices;
    vertices.reserve(segments * 3);

    // Use ComplexOrbit to sample the elliptical path (imaginary numbers!)
    for (int i = 0; i <= segments; i++) {
        const float t = (float)i / segments * planet.orbit.getPeriod();
        glm::vec2 pos = planet.orbit.getPosition(t);
        vertices.push_back(pos.x);
        vertices.push_back(0.0f);
        vertices.push_back(pos.y);
    }

    OrbitLine line;
    line.vertexCount = static_cast<int>(vertices.size() / 3);

    glGenVertexArrays(1, &line.VAO);
    glGenBuffers(1, &line.VBO);

    glBindVertexArray(line.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, line.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    orbitLines.push_back(line);
}

void PlanetSystem::renderOrbitLines(Shader& shader) {
    for (const auto& line : orbitLines) {
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        shader.setVec3("objectColor", glm::vec3(0.27f, 0.27f, 0.4f));
        shader.setFloat("emissive", 0.0f);

        glBindVertexArray(line.VAO);
        glDrawArrays(GL_LINE_LOOP, 0, line.vertexCount);
        glBindVertexArray(0);
    }
}
// createOrbitLine(*planet);