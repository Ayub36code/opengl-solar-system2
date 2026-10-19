

#pragma once
#include "config.h"

struct Star {
    glm::vec3 position;
    glm::vec3 color;
    float size;
    float phase;
};

class StarField {
public:
    StarField(int count = 10000);
    ~StarField();

    void update(float time);
    void render(Shader& shader);
    int getCount() const { return starCount; }

private:
    std::vector<Star> stars;
    unsigned int VAO, VBO;
    int starCount;
    float* positionData;
    float* colorData;
    float* sizeData;

    void generateStars(int count);
};

