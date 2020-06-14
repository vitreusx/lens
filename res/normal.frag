#version 460 core
out vec4 finalCol;
uniform vec3 color;

void main() {
    finalCol = vec4(color, 1.0);
}
