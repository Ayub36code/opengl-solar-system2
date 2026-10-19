
#include "../include/config.h"

StarField::StarField(int count) : starCount(count) {
    generateStars(count);

    // Setup vertex data
    positionData = new float[count * 3];
    colorData = new float[count * 3];
    sizeData = new float[count];

    for (int i = 0; i < count; i++) {
        positionData[i * 3] = stars[i].position.x;
        positionData[i * 3 + 1] = stars[i].position.y;
        positionData[i * 3 + 2] = stars[i].position.z;
        colorData[i * 3] = stars[i].color.x;
        colorData[i * 3 + 1] = stars[i].color.y;
        colorData[i * 3 + 2] = stars[i].color.z;
        sizeData[i] = stars[i].size;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count * (3 + 3 + 1) * sizeof(float), nullptr, GL_STATIC_DRAW);

    // Position
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * 3 * sizeof(float), positionData);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glBufferSubData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), count * 3 * sizeof(float), colorData);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(count * 3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Size
    glBufferSubData(GL_ARRAY_BUFFER, count * 6 * sizeof(float), count * sizeof(float), sizeData);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(count * 6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

StarField::~StarField() {
    delete[] positionData;
    delete[] colorData;
    delete[] sizeData;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void StarField::generateStars(int count) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    stars.resize(count);

    for (int i = 0; i < count; i++) {
        // Random spherical distribution (random numbers!)
        float radius = 300.0f + dis(gen) * 700.0f;
        float theta = dis(gen) * 2.0f * 3.14159f;
        float phi = acos(2.0f * dis(gen) - 1.0f);

        // Spherical to Cartesian with perturbation (imaginary numbers)
        float r1 = radius * (1.0f + 0.01f * sin(theta * 5.0f) * cos(phi * 3.0f));
        stars[i].position = glm::vec3(
            r1 * sin(phi) * cos(theta),
            r1 * cos(phi),
            r1 * sin(phi) * sin(theta)
        );

        // Random colors (blue-white to warm)
        float temp = 0.6f + dis(gen) * 0.4f;
        stars[i].color = glm::vec3(
            temp * (0.8f + dis(gen) * 0.2f),
            temp * (0.7f + dis(gen) * 0.3f),
            1.0f
        );

        stars[i].size = 0.5f + dis(gen) * 2.0f;
        stars[i].phase = dis(gen) * 2.0f * 3.14159f;
    }
}

void StarField::update(float time) {
    // Twinkling and subtle motion using imaginary numbers
    for (int i = 0; i < starCount; i++) {
        // Only move a subset for visual interest
        if (i % 7 == 0) {
            float drift = 0.02f * sin(time * 0.1f + stars[i].phase);
            positionData[i * 3] += drift * 0.001f;
            positionData[i * 3 + 1] += drift * 0.001f * cos(stars[i].phase);
        }
        // Twinkling effect (vary size)
        float twinkle = 1.0f + 0.3f * sin(time * 2.0f + stars[i].phase);
        sizeData[i] = (0.5f + stars[i].size * 0.5f) * twinkle;
    }

    // Update buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, starCount * 3 * sizeof(float), positionData);
    glBufferSubData(GL_ARRAY_BUFFER, starCount * 6 * sizeof(float), starCount * sizeof(float), sizeData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void StarField::render(Shader& shader) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, starCount);
    glBindVertexArray(0);
}