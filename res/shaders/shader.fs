#version 330 core

in vec2 uvPosition;
out vec3 color;

uniform sampler2D uTextureSampler;


void main() {
    color = texture(uTextureSampler, uvPosition).rgb;
}