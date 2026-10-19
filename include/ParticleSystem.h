
#pragma once
#include "config.h"


struct Particle {
    float t;
    float phase;
    glm::vec3 position;
    glm::vec3 color;
    float size;
};

class ParticleSystem {
public:
    explicit ParticleSystem(int count = 500);
    ~ParticleSystem();

    void update(float time);
    void render(Shader& shader);
    int getCount() const { return particleCount; }
    void reset();


private:
    std::vector<Particle> particles;
    unsigned int VAO, VBO;
    int particleCount;
    float* positionData;
    float* colorData;
    float* sizeData;

    void generateParticles(int count);
};


