#version 330 core
in vec3 Color;
out vec4 FragColor;

void main() {
    // Circular point
    vec2 center = gl_PointCoord - vec2(0.5);
    float dist = length(center);
    if (dist > 0.5) discard;

    float alpha = 1.0 - smoothstep(0.3, 0.5, dist);
    FragColor = vec4(Color, alpha);
}