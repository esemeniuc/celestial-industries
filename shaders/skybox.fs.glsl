#version 330
in vec3 texture_coordinates;
uniform samplerCube cube_texture;

// Output color
layout(location = 0) out  vec4 fragmentColour;

void main() {
	fragmentColour = texture(cube_texture, texture_coordinates);
}
