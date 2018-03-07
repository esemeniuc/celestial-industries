#version 330
// From vertex shader
in vec2 vs_texcoord;
in vec3 vs_normal;
in vec3 vs_position;
in vec3 vs_lightVector;

uniform material {
    uniform bool hasDiffuseMap;
    uniform vec3 ambient;
    uniform vec3 diffuse;
    uniform vec3 specular;
}

uniform sampler2D diffuseMapSampler;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	vec4 lightColor = vec4(1.0,1.0,1.0, 1.0);
	float angleToLight = clamp( dot(normalize(vs_normal), normalize(vs_lightVector)), 0, 1);
	vec4 diffuseColor;
	if(hasDiffuseMap){
		diffuseColor = vec4(diffuse, 1.0)*texture(diffuseMapSampler, vs_texcoord);
	} else {
		diffuseColor = vec4(diffuse, 1.0);
	}
	if(angleToLight < 0.05){
		// Shitty way of simulating ambient light
		angleToLight = 0.05;
	}
	color = vec4(ambient,1.0)+lightColor*angleToLight*diffuseColor;
}
