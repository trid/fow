#version 330 core

layout(location = 0) in vec3 in_vertexPosition;
layout(location = 1) in vec2 in_uvPosition;

out vec2 uvPosition;

void main() {
    gl_Position.xyz = in_vertexPosition;
    gl_Position.w = 1.0;
    uvPosition = in_uvPosition;
}