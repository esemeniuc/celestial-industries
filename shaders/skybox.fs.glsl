#version 330 
in vec3 texture_coordinates;
uniform samplerCube cube_texture;

void main() {
	gl_FragColor = texture(cube_texture, texture_coordinates);
	//gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}