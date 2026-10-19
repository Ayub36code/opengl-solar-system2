
#pragma once
#include "../../include/config.h"

/**
 * Represents an elliptical orbit using complex numbers (imaginary numbers!)
 * z = a * cos(t) + i * b * sin(t)
 * This is the "imaginary numbers" part of the project
 */
class ComplexOrbit {
public:
    float semiMajorAxis;
    float semiMinorAxis;
    float angularSpeed;
    float phase;

    explicit ComplexOrbit(float a = 10.0f, float b = 10.0f, float speed = 0.5f, float phase = 0.0f)
        : semiMajorAxis(a), semiMinorAxis(b), angularSpeed(speed), phase(phase) {}

    /**
     * Get position on the orbit at time t
     * Uses the complex exponential: z = a * e^(i*θ) + b * e^(-i*θ)
     * This is the imaginary number math!
     */
    glm::vec2 getPosition(float t) const {
        const float angle = t * angularSpeed + phase;

        // Complex multiplication: (a + ib) * (cosθ + i*sinθ)
        // Real part = a*cosθ - b*sinθ
        // Imag part = a*sinθ + b*cosθ
        const float real = semiMajorAxis * cos(angle) - semiMinorAxis * sin(angle);
        const float imag = semiMajorAxis * sin(angle) + semiMinorAxis * cos(angle);

        // Add some chaotic perturbation using more imaginary numbers
        const float chaos = 0.002f * sin(t * 0.7f + phase);
        const float chaos2 = 0.002f * cos(t * 0.5f + phase * 1.3f);

        return glm::vec2(real + chaos, imag + chaos2);
    }

    /**
     * Get velocity at time t (derivative of complex orbit)
     * v = d/dt [a*cos(θ) + i*b*sin(θ)] = -a*sin(θ) + i*b*cos(θ)
     */
    glm::vec2 getVelocity(const float t) const {
        const float angle = t * angularSpeed + phase;
        return {
            -semiMajorAxis * angularSpeed * std::sin(angle),
            semiMinorAxis * angularSpeed * std::cos(angle)
        };
    }

    // Append inside the class:
    glm::vec2 getPositionEuler(float t) const;
    glm::vec2 getConjugatePosition(float t) const;
    float getMagnitude(float t) const;
    float getArgument(float t) const;

    static glm::vec2 complexMultiply(const glm::vec2& z1, const glm::vec2& z2);
    static glm::vec2 complexDivide(const glm::vec2& z1, const glm::vec2& z2);
    static glm::vec2 complexExp(const glm::vec2& z);

    /**
     * Get the orbital period using imaginary frequency
     * T = 2π / ω
     */
    float getPeriod() const {
        return (2.0f * 3.14159f) / angularSpeed;
    }

    /**
     * Compute the eccentricity using complex parameters
     * e = sqrt(1 - (b²/a²))
     */
    float getEccentricity() const {
        return std::sqrt(1.0f - (semiMinorAxis * semiMinorAxis) / (semiMajorAxis * semiMajorAxis));
    }
};


