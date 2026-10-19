
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "Shader.h"

struct Asteroid {
    glm::vec3 position;
    glm::quat orientation;
    float angle;
    float radius;
    float speed;
    float heightOffset;
    float phase;
    float wobble;
    float size;
};

class AsteroidBelt {
public:
    AsteroidBelt(int count = 2000);
    ~AsteroidBelt();

    void update(float time);
    void render(Shader& shader);
    int getCount() const { return asteroidCount; }
    glm::vec3 getAsteroidPosition(int index) const;

private:
    std::vector<Asteroid> asteroids;
    unsigned int VAO{}, VBO{};
    int asteroidCount;
    float* positionData;
    float* colorData;
    float* sizeData;

    void generateAsteroids(int count);
    glm::mat4 getModelMatrix(const Asteroid& ast);
};

