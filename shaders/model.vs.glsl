#version 330 
// Input attributes
in vec3 in_position;

// Passed to fragment shader
out vec3 vs_color;

// Application data
uniform mat4 mvp;


void main()
{
	gl_Position = mvp * vec4(in_position, 1);
	vs_color = in_position;
}