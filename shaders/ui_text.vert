#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;


out vec2 texCoord;

uniform mat4 pro;
uniform mat4 model;

void main()
{
    gl_Position = pro * model * vec4(aPos, 0.0, 1.0);
    texCoord    = vec2(aTexCoord.x, 1-aTexCoord.y);
}