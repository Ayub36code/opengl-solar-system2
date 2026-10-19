#include "../include/config.h"


// Most methods are inline in the header. This file implements
// the more advanced complex-number operations.

/**
 * Evaluate the orbit using Euler's formula directly:
 * z(t) = A * e^(i*ω*t) where A is a complex amplitude (a + ib)
 * This is the purest "imaginary number" representation.
 */
glm::vec2 ComplexOrbit::getPositionEuler(float t) const {
    const float angle = t * angularSpeed + phase;

    // Complex amplitude A = a + i*b
    const float aReal = semiMajorAxis;
    const float aImag = semiMinorAxis;

    // e^(i*angle) = cos(angle) + i*sin(angle)
    const float cosA = cos(angle);
    const float sinA = sin(angle);

    // Complex multiplication: (a + ib)(cosA + i*sinA)
    //  = (a*cosA - b*sinA) + i(a*sinA + b*cosA)
    const float realPart = aReal * cosA - aImag * sinA;
    const float imagPart = aReal * sinA + aImag * cosA;

    return {realPart, imagPart};
}

/**
 * Compute the conjugate of the complex position.
 * The conjugate of z = x + iy is z̄ = x - iy.
 * This is useful for mirroring orbits (retrograde motion).
 */
glm::vec2 ComplexOrbit::getConjugatePosition(float t) const {
    const glm::vec2 pos = getPosition(t);
    return {pos.x, -pos.y};
}

/**
 * Complex magnitude (modulus): |z| = sqrt(x² + y²)
 * For our orbit, this gives the distance from the focus.
 */
float ComplexOrbit::getMagnitude(float t) const {
    glm::vec2 pos = getPosition(t);
    return sqrt(pos.x * pos.x + pos.y * pos.y);
}

/**
 * Complex argument (phase angle): arg(z) = atan2(y, x)
 * This gives the true anomaly of the orbit.
 */
float ComplexOrbit::getArgument(float t) const {
    const glm::vec2 pos = getPosition(t);
    return atan2(pos.y, pos.x);
}

/**
 * Multiply two complex numbers:
 * (a + ib)(c + id) = (ac - bd) + i(ad + bc)
 */
glm::vec2 ComplexOrbit::complexMultiply(const glm::vec2& z1, const glm::vec2& z2) {
    return {
        z1.x * z2.x - z1.y * z2.y,
        z1.x * z2.y + z1.y * z2.x
    };
}

/**
 * Divide two complex numbers:
 * (a + ib)/(c + id) = ((ac + bd) + i(bc - ad)) / (c² + d²)
 */
glm::vec2 ComplexOrbit::complexDivide(const glm::vec2& z1, const glm::vec2& z2) {
    float denom = z2.x * z2.x + z2.y * z2.y;
    if (denom < 1e-6f) return glm::vec2(0.0f);
    return {
        (z1.x * z2.x + z1.y * z2.y) / denom,
        (z1.y * z2.x - z1.x * z2.y) / denom
    };
}

/**
 * Complex exponential: e^z where z = x + iy
 * e^(x + iy) = e^x * (cos(y) + i*sin(y))
 */
glm::vec2 ComplexOrbit::complexExp(const glm::vec2& z) {
    const float expReal = exp(z.x);
    return {
        expReal * cos(z.y),
        expReal * sin(z.y)
    };
}