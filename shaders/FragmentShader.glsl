#version 330 core

in vec2 fragCoords;
in float height;

out vec4 FragColor;

float noise(vec2 coords) {
    vec2 seed = vec2(75.0, 11.0);
    float projection = dot(coords, seed);
    float oscillation = sin(projection);
    return fract(oscillation * 3100);
}


void main() {
    float detail = noise(fragCoords * 10.0);
    vec3 oceanColor = vec3(0.0, 0.459, 0.89);
    vec3 soilColor = vec3(0.19, 0.62, 0.12);
    vec3 rockyColor = vec3(0.54, 0.27, 0.07);
    vec3 grassShade = vec3(0.36, 0.25, 0.20);
    vec3 rockColor = vec3(0.5, 0.5, 0.5);
    vec3 finalColor;

    if (height < -0.511) {
        finalColor = mix(oceanColor, soilColor, detail);
    } else if (height < 0.2) {
        finalColor = mix(soilColor, rockyColor, detail);
    } else if (height < 0.8) {
        finalColor = mix(rockyColor, grassShade, detail);
    } else {
        finalColor = mix(grassShade, rockColor, detail);
    }

    FragColor = vec4(finalColor, 1.0);
}
