#version 330 
// Input attributes
in vec3 in_position;
in vec3 in_texcoord;

// Passed to fragment shader
out vec3 vs_color;
out vec3 vs_texcoord;
out float vs_hasDiffuseMap; // Yes it literally is that stupid as to disallow bool/int outs

// Application data
uniform mat4 mvp;
uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform bool hasDiffuseMap;

void main()
{
	gl_Position = mvp * vec4(in_position, 1);
	vs_color = material_diffuse;
	if(hasDiffuseMap){
		vs_hasDiffuseMap = 1.0;
	} else {
		vs_hasDiffuseMap = 0.0;
	}
	vs_texcoord = in_texcoord;
}