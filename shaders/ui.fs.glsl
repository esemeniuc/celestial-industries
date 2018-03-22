#version 410

uniform sampler2D theTexture;

in vec2 textureCoordinateFromVertexShader;
layout(location = 0) out  vec4 color;

void main() {
    color = vec4(0.5, 0.5, 1.0, 1.0);
}
