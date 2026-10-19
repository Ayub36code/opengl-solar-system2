#version 330 core
in vec3 Normal;

uniform float time;

out vec4 FragColor;

void main() {
    // Edge glow effect
    vec3 viewDir = vec3(0.0, 0.0, 1.0);
    float intensity = pow(0.65 - dot(Normal, viewDir), 2.0);
    float pulse = 0.8 + 0.2 * sin(time * 0.5);
    FragColor = vec4(1.0, 0.6, 0.0, intensity * 0.6 * pulse);
}