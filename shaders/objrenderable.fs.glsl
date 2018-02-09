#version 330
// From vertex shader
in vec2 vs_texcoord;
in vec3 vs_normal;
in vec3 vs_position;
in vec3 vs_lightVector;

uniform sampler2D diffuseMapSampler;
uniform bool hasDiffuseMap;
uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	vec4 lightColor = vec4(1.0,1.0,1.0, 1.0);
	float angleToLight = clamp( dot(normalize(vs_normal), normalize(vs_lightVector)), 0, 1);
	vec4 diffuseColor;
	if(hasDiffuseMap){
		diffuseColor = vec4(material_diffuse, 1.0)*texture(diffuseMapSampler, vs_texcoord);
	} else {
		diffuseColor = vec4(material_diffuse, 1.0);
	}
	if(angleToLight < 0.05){
		// Shitty way of simulating ambient light
		angleToLight = 0.05;
	}
	color = vec4(material_ambient,1.0)+lightColor*angleToLight*diffuseColor;
}
