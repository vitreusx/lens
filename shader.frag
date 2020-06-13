#version 330 core
in vec3 fsCol;
out vec4 finalCol;

void main()
{
    finalCol = vec4(fsCol, 1.0);
}
