#version 330 core
layout (location = 0) in vec3 vsPos;
layout (location = 1) in vec3 vsCol;

out vec3 fsCol;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * model * vec4(vsPos, 1.0);
    fsCol = vsCol;
}
