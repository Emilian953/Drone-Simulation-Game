#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 fragCoords;
out float height;

float gradientNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    // Generez gradiente
    float n00 = dot(fract(sin(i) * vec2(123, 123)), f);
    float n10 = dot(fract(sin(i + vec2(1, 0)) * vec2(123, 123)), f - vec2(1, 0));
    float n01 = dot(fract(sin(i + vec2(0, 1)) * vec2(123, 123)), f - vec2(0, 1));
    float n11 = dot(fract(sin(i + vec2(1, 1)) * vec2(123, 123)), f - vec2(1, 1));

    // Interpolez
    vec2 u = f * f * (3.0 - 2.0 * f);
    float nx0 = mix(n00, n10, u.x);
    float nx1 = mix(n01, n11, u.x);
    return mix(nx0, nx1, u.y);
}

void main() {
    const float frequency = 0.1;
    const float amplitude = 4.0;

    vec2 scaledCoords = position.xz * frequency;
    float n = gradientNoise(scaledCoords) * amplitude;
    vec3 newPosition = vec3(position.x, position.y + n, position.z);

    fragCoords = texCoords;
    height = newPosition.y;

    gl_Position = Projection * View * Model * vec4(newPosition, 1.0);
}
