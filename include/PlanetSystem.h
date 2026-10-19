

#pragma once
#include "../../include/config.h"
#include <glm/gtc/quaternion.hpp>
#
#include "ComplexOrbit.h"


struct PlanetData {
    std::string name;
    float radius;
    float orbitRadius;
    float speed;
    glm::vec3 color;
    float tilt;
    bool hasRings;
};

struct Planet {
    glm::vec3 position;
    glm::quat orientation;
    glm::quat tiltQuat;
    float rotationAngle;
    ComplexOrbit orbit;
    PlanetData data;
    unsigned int VAO, VBO, EBO;
    unsigned int ringVAO, ringVBO, ringEBO;
    int indexCount;
    int ringIndexCount;
};

class PlanetSystem {
public:
    PlanetSystem();
    ~PlanetSystem();

    void createAll();
    void update(float deltaTime, float elapsed);
    void render(Shader& shader);
    void renderGlow(Shader& shader);
    Planet* getPlanet(const std::string& name);
    void setSpeed(float multiplier);
    std::vector<std::unique_ptr<Planet>>& getAllPlanets() { return planets; }
    void renderOrbitLinesPublic(Shader& shader) { renderOrbitLines(shader); }

private:
    std::vector<std::unique_ptr<Planet>> planets;
    unsigned int sunVAO{}, sunVBO{}, sunEBO{};
    unsigned int glowVAO{}, glowVBO{}, glowEBO{};
    int sunIndexCount{};
    int glowIndexCount{};
    float speedMultiplier = 1.0f;
    // Add to private members:
    struct OrbitLine {
        unsigned int VAO, VBO;
        int vertexCount;
    };
    std::vector<OrbitLine> orbitLines;

    void createSun();
    void createPlanetMesh(Planet& planet);
    void createRingMesh(Planet& planet);
    void createOrbitLine(const Planet& planet);
    void setupMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO,
                   float* vertices, int vertCount, unsigned int* indices, int idxCount);
    void renderOrbitLines(Shader& shader);
};

