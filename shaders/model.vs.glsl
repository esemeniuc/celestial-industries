#version 330 
// Input attributes
in vec3 in_position;
in vec2 in_texcoord;
in vec3 in_normal;

// Passed to fragment shader
out vec2 vs_texcoord;
out vec3 vs_normal;
out vec3 vs_position;
out vec3 vs_lightVector;

// Application data
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(in_position, 1);
	vs_texcoord = in_texcoord;
	vs_normal = in_normal;

	vec3 lightPosition = vec3( 0,10, -15 );
	vs_lightVector = lightPosition-vs_position;
}