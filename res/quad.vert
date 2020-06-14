#version 460 core
layout (location = 0) in vec3 vsPos;
layout (location = 2) in vec2 vsTexUV;

out vec2 fsTexUV;

void main() {
    gl_Position = vec4(vsPos, 1.0);
    fsTexUV = vec2(vsTexUV.x, vsTexUV.y);
}
