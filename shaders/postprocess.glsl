#version 450 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool bloom;
uniform float exposure;

void main() {
    vec3 color = texture(screenTexture, TexCoords).rgb;

    if (bloom) {
        // Simple bloom approximation
        vec3 bloomColor = color * 0.5;
        color += bloomColor;
    }

    // Tone mapping
    color = vec3(1.0) - exp(-color * exposure);

    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}