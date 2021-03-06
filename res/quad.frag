#version 460 core
out vec4 finalCol;
in vec2 fsTexUV;

uniform sampler2D rayTex;

void main() {
    finalCol = texture(rayTex, fsTexUV);
}
