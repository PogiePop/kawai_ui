#version 460 core
layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTex;

out vec2 vUv;

uniform mat4 pro;
uniform mat4 model;

void main()
{
    gl_Position = pro * model * vec4(vPos, 0.0, 1.0);
    vUv = vTex;
}