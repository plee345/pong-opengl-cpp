#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec2 position;
uniform vec2 startingOffset;


void main()
{
    gl_Position = vec4((aPos.x + position.x + startingOffset.x), (aPos.y + position.y + startingOffset.y), aPos.z, 1.0);
}