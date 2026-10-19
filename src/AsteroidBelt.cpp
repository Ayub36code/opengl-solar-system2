#include "../include/config.h"


AsteroidBelt::AsteroidBelt(const int count) : asteroidCount(count) {
    generateAsteroids(count);

    positionData = new float[count * 3];
    colorData = new float[count * 3];
    sizeData = new float[count];

    for (int i = 0; i < count; i++) {
        positionData[i * 3] = asteroids[i].position.x;
        positionData[i * 3 + 1] = asteroids[i].position.y;
        positionData[i * 3 + 2] = asteroids[i].position.z;
        colorData[i * 3] = 0.53f + 0.2f * static_cast<float>(i % 3) / 3.0f;
        colorData[i * 3 + 1] = 0.40f + 0.2f * static_cast<float>(i % 5) / 5.0f;
        colorData[i * 3 + 2] = 0.27f + 0.1f * static_cast<float>(i % 7) / 7.0f;
        sizeData[i] = asteroids[i].size;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count * (3 + 3 + 1) * sizeof(float), nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, count * 3 * sizeof(float), positionData);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBufferSubData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), count * 3 * sizeof(float), colorData);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(count * 3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBufferSubData(GL_ARRAY_BUFFER, count * 6 * sizeof(float), count * sizeof(float), sizeData);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(count * 6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

AsteroidBelt::~AsteroidBelt() {
    delete[] positionData;
    delete[] colorData;
    delete[] sizeData;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void AsteroidBelt::generateAsteroids(int count) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    float innerRadius = 17.0f;
    float outerRadius = 19.0f;
    float height = 1.2f;

    asteroids.resize(count);

    for (int i = 0; i < count; i++) {
        // Random position in the belt (random numbers!)
        const float angle = dis(gen) * 2.0f * 3.14159f;
        const float radius = innerRadius + dis(gen) * (outerRadius - innerRadius);
        const float heightOffset = (dis(gen) - 0.5f) * height;

        asteroids[i].angle = angle;
        asteroids[i].radius = radius;
        asteroids[i].heightOffset = heightOffset;
        asteroids[i].speed = 0.2f + dis(gen) * 0.3f;
        asteroids[i].phase = dis(gen) * 2.0f * 3.14159f;
        asteroids[i].wobble = 0.1f + dis(gen) * 0.2f;
        asteroids[i].size = 0.05f + dis(gen) * 0.15f;

        asteroids[i].position = glm::vec3(
            radius * cos(angle),
            heightOffset,
            radius * sin(angle)
        );

        // Random rotation using quaternions!
        asteroids[i].orientation = QuaternionUtils::random();
    }
}

void AsteroidBelt::update(const float time) {
    for (int i = 0; i < asteroidCount; i++) {
        auto& ast = asteroids[i];

        // Orbit with slight chaos (imaginary numbers influence)
        const float angle = ast.angle + time * ast.speed * 0.05f;
        const float radiusVar = ast.radius + 0.1f * sin(time * ast.wobble + ast.phase);

        ast.position.x = radiusVar * cos(angle);
        ast.position.z = radiusVar * sin(angle);
        ast.position.y = ast.heightOffset + 0.2f * sin(time * 0.5f + ast.phase);

        // Slow rotation using quaternions
        glm::vec3 axis = glm::normalize(glm::vec3(
            sin(time * 0.1f + ast.phase),
            cos(time * 0.15f + ast.phase * 1.3f),
            sin(time * 0.12f + ast.phase * 0.7f)
        ));
        glm::quat rotQuat = glm::angleAxis(0.01f, axis);
        ast.orientation = rotQuat * ast.orientation;

        // Update buffer data
        positionData[i * 3] = ast.position.x;
        positionData[i * 3 + 1] = ast.position.y;
        positionData[i * 3 + 2] = ast.position.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, asteroidCount * 3 * sizeof(float), positionData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AsteroidBelt::render(Shader& shader) {
    // Render as points (can be upgraded to instanced rendering for cubes)
    glPointSize(2.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, asteroidCount);
    glBindVertexArray(0);
}
glm::vec3 AsteroidBelt::getAsteroidPosition(int index) const {
    if (index < 0 || index >= asteroidCount) return glm::vec3(0.0f);
    return asteroids[index].position;
}