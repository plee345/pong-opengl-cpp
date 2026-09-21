#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec2 position;
uniform vec2 vertexOffset;

uniform int shapeWho;
uniform float aspectRatio;

void cube();
void rectangle();

void main()
{
    if (shapeWho == 0)
    {
        cube();
    }
    if (shapeWho == 1)
    {
        rectangle();
    }
}

void rectangle()
{
    gl_Position = vec4((aPos.x / aspectRatio) + position.x, ((aPos.y / aspectRatio) + position.y), aPos.z, 1.0);
}

void cube()
{
    gl_Position = vec4((aPos.x / aspectRatio) + position.x, ((aPos.y / aspectRatio) + position.y), aPos.z, 1.0);
}