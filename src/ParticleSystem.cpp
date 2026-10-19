#include "../include/config.h"
ParticleSystem::ParticleSystem(const int count) : particleCount(count) {
    generateParticles(count);

    positionData = new float[count * 3];
    colorData = new float[count * 3];
    sizeData = new float[count];

    for (int i = 0; i < count; i++) {
        positionData[i * 3] = particles[i].position.x;
        positionData[i * 3 + 1] = particles[i].position.y;
        positionData[i * 3 + 2] = particles[i].position.z;
        colorData[i * 3] = particles[i].color.x;
        colorData[i * 3 + 1] = particles[i].color.y;
        colorData[i * 3 + 2] = particles[i].color.z;
        sizeData[i] = particles[i].size;
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

ParticleSystem::~ParticleSystem() {
    delete[] positionData;
    delete[] colorData;
    delete[] sizeData;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ParticleSystem::generateParticles(int count) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    particles.resize(count);

    for (int i = 0; i < count; i++) {
        const float t = static_cast<float>(i) / count;
        const float radius = 25.0f + t * 8.0f;
        const float angle = t * 3.0f + 0.5f;

        particles[i].t = t;
        particles[i].phase = dis(gen) * 2.0f * 3.14159f;
        particles[i].position = glm::vec3(
            radius * cos(angle),
            0.5f * sin(t * 10.0f) * (1.0f - t),
            radius * sin(angle)
        );

        float brightness = 1.0f - t;
        particles[i].color = glm::vec3(
            1.0f * brightness,
            0.8f * brightness,
            0.3f * brightness
        );

        particles[i].size = (1.0f - t) * 1.5f + 0.2f;
    }
}

void ParticleSystem::update(float time) {
    float speed = 0.3f;

    for (int i = 0; i < particleCount; i++) {
        auto& p = particles[i];
        float t = fmod(p.t + time * 0.02f * speed, 1.0f);
        float radius = 25.0f + t * 8.0f;
        float angle = t * 3.0f + 0.5f + 0.2f * sin(time * 0.1f + p.phase);

        p.position = glm::vec3(
            radius * cos(angle),
            1.5f * sin(t * 10.0f + time * 0.2f) * (1.0f - t) + 0.5f,
            radius * sin(angle)
        );

        float brightness = 1.0f - t;
        p.color = glm::vec3(
            1.0f * brightness,
            0.8f * brightness,
            0.3f * brightness
        );
        p.size = (1.0f - t) * 1.5f + 0.2f;

        positionData[i * 3] = p.position.x;
        positionData[i * 3 + 1] = p.position.y;
        positionData[i * 3 + 2] = p.position.z;
        colorData[i * 3] = p.color.x;
        colorData[i * 3 + 1] = p.color.y;
        colorData[i * 3 + 2] = p.color.z;
        sizeData[i] = p.size;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 3 * sizeof(float), positionData);
    glBufferSubData(GL_ARRAY_BUFFER, particleCount * 3 * sizeof(float), particleCount * 3 * sizeof(float), colorData);
    glBufferSubData(GL_ARRAY_BUFFER, particleCount * 6 * sizeof(float), particleCount * sizeof(float), sizeData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::render(Shader& shader) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glPointSize(3.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, particleCount);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}
void ParticleSystem::reset() {
    for (int i = 0; i < particleCount; i++) {
        particles[i].t = (float)i / particleCount;
    }
}
