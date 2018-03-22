#version 410

uniform mat4 modelViewProjection;

in vec3 position;
in vec2 textureCoordinate;

out vec2 textureCoordinateFromVertexShader;


void main() {
    // tells us which particle we're dealing with - we're using it to seed the RNG
    // consistently for each particle
    // gl_InstanceID;
    textureCoordinateFromVertexShader = textureCoordinate;

    gl_Position = vec4(position, 1.0);
}
