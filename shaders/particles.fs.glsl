#version 410

uniform sampler2D theTexture;

in vec2 textureCoordinateFromVertexShader;
layout(location = 0) out  vec4 color;

void main() {
    color = texture(theTexture, textureCoordinateFromVertexShader) * vec4(1.0, 1.0, 1.0, 0.8);
}
